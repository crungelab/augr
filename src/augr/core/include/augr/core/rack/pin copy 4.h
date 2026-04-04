#pragma once

#include <functional>
#include <list>
#include <algorithm>
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
    virtual ~Connection() = default;

    Pin *output_;
    Pin *input_;
};

template <typename T = void> class ConnectionT : public Connection {
public:
    typedef std::function<void(T)> TCallback;

    ConnectionT(Pin &output, Pin &input, const TCallback &callback)
        : Connection(output, input), callback_(callback) {}
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
    virtual Connection *Connect(Pin &other) = 0;
    virtual void Disconnect(Connection &connection) = 0;

    Node &node() { return *(Node *)owner_; }
    std::string name_;
    std::vector<Wire *> wires_;
};

// Forward declarations
template <typename T, typename TBase = Pin> class InputT;
template <typename T, typename TBase = Pin> class OutputT;

template <typename T, typename TBase> class OutputT : public TBase {
    using TCallback = std::function<void(T)>;
    using TConnection = ConnectionT<T>;
    using TInput = InputT<T, TBase>;

public:
    template <typename... Args>
    OutputT(Node &node, std::string name, Args &&...args)
        : TBase(node, name, std::forward<Args>(args)...) {}

    Connection *Connect(Pin &_input) override {
        TInput *input = dynamic_cast<TInput *>(&_input);
        if (!input) return nullptr;
        return Subscribe(*input, [input](T value) { input->Write(value); });
    }

    void Disconnect(Connection &connection) override {
        Unsubscribe(connection);
    }

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

    void Unsubscribe(Connection &connection) {
        auto *typed = dynamic_cast<TConnection *>(&connection);
        if (!typed) return;

        connections_.remove(typed);
        delete typed;
    }

    virtual void Write(T value) {
        value_ = value;
        Publish(value);
    }

    T Read() { return value_; }

    std::list<TConnection *> connections_;
    T value_;
};

template <typename T, typename TBase> class InputT : public TBase {
    using TConnection = ConnectionT<T>;
    using TOutput = OutputT<T, TBase>;

public:
    template <typename... Args>
    InputT(Node &node, std::string name, Args &&...args)
        : TBase(node, name, std::forward<Args>(args)...) {}

    Connection *Connect(Pin &_output) override {
        TOutput *output = dynamic_cast<TOutput *>(&_output);
        if (!output) return nullptr;
        return output->Connect(*this);
    }

    void Disconnect(Connection &connection) override {
        connection.output_->Disconnect(connection);
    }

    virtual void Write(T value) {
        value_ = value;
    }

    T Read() { return value_; }

    T value_;
};

} // namespace augr