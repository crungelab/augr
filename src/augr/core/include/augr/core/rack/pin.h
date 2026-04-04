#pragma once

#include <functional>
#include <list>
#include <string>
#include <vector>

#include <augr/core/part.h>

namespace augr {

class Node;
class Wire;
class Pin;

class Connection {
public:
    Connection(Pin &output, Pin &input) : output_(&output), input_(&input) {}
    Pin *output_;
    Pin *input_;
};

template <typename T = void> class ConnectionT : public Connection {
public:
    typedef std::function<void(T)> TCallback;

    ConnectionT(Pin &output, Pin &input, const TCallback &callback) : Connection(output, input), callback_(callback) {}
    TCallback callback_;
};

class Pin : public Part {
public:
    Pin(Node &node, std::string name) : name_(name) {}
    virtual void AddWire(Wire &wire) { wires_.push_back(&wire); }
    virtual void RemoveWire(Wire &wire) {
        auto it = std::remove(wires_.begin(), wires_.end(), &wire);
        wires_.erase(it, wires_.end());
    }
    virtual Connection *Connect(Pin &output) = 0;
    virtual void Disconnect(Connection &connection) = 0;
    virtual void DisconnectInput(Pin &input) = 0;

    // Data members
    Node &node() { return *(Node *)owner_; }
    std::string name_;
    std::vector<Wire *> wires_;
};

template <typename T> class PinT : public Pin {
    using TCallback = std::function<void(T)>;
    using TConnection = ConnectionT<T>;

public:
    PinT(Node &node, std::string name) : Pin(node, name) {}
    Connection *Connect(Pin &_output) override {
        PinT<T> *output = dynamic_cast<PinT<T> *>(&_output);
        return output->Subscribe(*this, [this](T value) { this->Write(value); });
    }
    void Disconnect(Connection &connection) override {
        connection.output_->DisconnectInput(*this);
    }
    void DisconnectInput(Pin &input) override {
        for (auto it = connections_.begin(); it != connections_.end(); ++it) {
            TConnection *connection = dynamic_cast<TConnection *>(*it);
            if (connection && connection->input_ == &input) {
                Unsubscribe(connection);
                break;
            }
        }
    }
    virtual void Write(T value) {
        value_ = value;
        Publish(value);
    }
    T Read() { return value_; }
    // Connections
    std::list<TConnection *> connections_;
    void Publish(T value) {
        for (auto connection : connections_) {
            connection->callback_(value);
        }
    }
    TConnection *Subscribe(Pin &pin, const TCallback &callback) {
        TConnection *connection = new TConnection(*this, pin, callback);
        connections_.push_back(connection);
        return connection;
    }
    void Unsubscribe(TConnection *connection) {
        connections_.remove(connection);
        delete connection;
    }
    // Accessors
    // Data members
    T value_;
};

} // namespace augr