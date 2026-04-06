#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <augr/core/part.h>
#include <augr/core/rack/pin.h>

namespace augr {

class Node;
class Wire;
class Connector;

// Base connection — holds connector-level pointers
class Connection {
public:
    Connection(Connector &output, Connector &input)
        : output_(&output), input_(&input) {}
    virtual ~Connection() = default;

    Connector *output_;
    Connector *input_;
};

// Typed connection — also owns the input pin
template <typename T> class ConnectionT : public Connection {
public:
    using TCallback = std::function<void(T)>;

    ConnectionT(Connector &output, Connector &input, PinT<T> *pin,
                const TCallback &callback)
        : Connection(output, input), pin_(pin), callback_(callback) {}

    PinT<T> *pin_ = nullptr; // owned by InputT, identified here for removal
    TCallback callback_;
};

// Base connector — wiring, name, node ref
class Connector : public Part {
public:
    Connector(Node &node, std::string name) : name_(name) {}

    virtual void AddWire(Wire &wire) { wires_.push_back(&wire); }
    virtual void RemoveWire(Wire &wire) {
        auto it = std::remove(wires_.begin(), wires_.end(), &wire);
        wires_.erase(it, wires_.end());
    }

    virtual Connection *Connect(Connector &other) = 0;
    virtual void Disconnect(Connection &connection) = 0;

    Node &node() { return *(Node *)owner_; }

    std::string name_;
    std::vector<Wire *> wires_;
};

// Typed connector base — holds the primary pin
template <typename T, typename TBase = Connector>
class ConnectorT : public TBase {
public:
    template <typename... Args>
    ConnectorT(Node &node, std::string name, Args &&...args)
        : TBase(node, name, std::forward<Args>(args)...) {}

    T Read() const { return value_; }
    virtual void Write(T value) { value_ = value; }

    T value_{};
};

// Forward declarations
template <typename T, typename TBase = Connector> class InputT;
template <typename T, typename TBase = Connector> class OutputT;

// Output connector — publishes to all subscribers
template <typename T, typename TBase>
class OutputT : public ConnectorT<T, TBase> {
    using TCallback = std::function<void(T)>;
    using TConnection = ConnectionT<T>;
    using TInput = InputT<T, TBase>;

public:
    template <typename... Args>
    OutputT(Node &node, std::string name, Args &&...args)
        : ConnectorT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    Connection *Connect(Connector &input) override {
        // Connection is initiated from the input side
        return nullptr;
    }

    void Disconnect(Connection &connection) override {
        Unsubscribe(connection);
    }

    void Publish(T value) {
        for (auto *conn : connections_) {
            conn->callback_(value);
        }
    }

    TConnection *Subscribe(Connector &input, PinT<T> *pin,
                           const TCallback &callback) {
        auto *connection = new TConnection(*this, input, pin, callback);
        connections_.push_back(connection);
        return connection;
    }

    void Unsubscribe(Connection &connection) {
        auto *typed = dynamic_cast<TConnection *>(&connection);
        if (!typed)
            return;
        connections_.remove(typed);
        delete typed;
    }

    void Write(T value) override {
        ConnectorT<T, TBase>::Write(value);
        Publish(value);
    }

    std::list<TConnection *> connections_;
};

// Input connector — uses per-connection pins, mixes on Read
template <typename T, typename TBase>
class InputT : public ConnectorT<T, TBase> {
    using TConnection = ConnectionT<T>;
    using TOutput = OutputT<T, TBase>;

public:
    template <typename... Args>
    InputT(Node &node, std::string name, Args &&...args)
        : ConnectorT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    Connection *Connect(Connector &_output) override {
        TOutput *output = dynamic_cast<TOutput *>(&_output);
        if (!output)
            return nullptr;

        // Allocate a dedicated pin for this connection
        auto *pin = new PinT<T>();
        pins_.push_back(pin);

        return output->Subscribe(*this, pin, [this, pin](T value) {
            pin->Write(value);
            dirty_ = true;
        });
    }

    void Disconnect(Connection &connection) override {
        auto *typed = dynamic_cast<TConnection *>(&connection);
        if (typed) {
            // Remove and delete the pin owned by this connection
            auto it = std::find(pins_.begin(), pins_.end(), typed->pin_);
            if (it != pins_.end()) {
                delete *it;
                pins_.erase(it);
            }
        }
        // Delegate unsubscribe to the output
        connection.output_->Disconnect(connection);

        // If no connections remain, clear cached value and dirty flag
        if (pins_.empty()) {
            this->value_ = T{};
            dirty_ = false;
        }
    }

    virtual T Reduce() const {
        T mixed = pins_[0]->Read();
        for (size_t i = 1; i < pins_.size(); ++i)
            mixed += pins_[i]->Read();
        return mixed;
    }

    T Read() {
        if (dirty_) {
            this->value_ = (pins_.size() == 1 ? pins_[0]->Read() : Reduce());
            dirty_ = false;
        }
        return this->value_;
    }

    ~InputT() {
        for (auto *pin : pins_)
            delete pin;
    }

    std::vector<PinT<T> *> pins_;
    bool dirty_ = false;
};

} // namespace augr