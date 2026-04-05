#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <string>
#include <vector>

#include <augr/core/part.h>
#include <augr/core/rack/pin.h>

namespace augr {

class Node;
class Wire;
class Connector;

class Connection {
public:
    Connection(Connector &output, Connector &input)
        : output_(&output), input_(&input) {}
    virtual ~Connection() = default;

    Connector *output_;
    Connector *input_;
};

template <typename T = void> class ConnectionT : public Connection {
public:
    typedef std::function<void(T)> TCallback;

    ConnectionT(Connector &output, Connector &input, const TCallback &callback)
        : Connection(output, input), callback_(callback) {}
    TCallback callback_;
};

class Connector : public Part {
public:
    Connector(Node &node, std::string name) : name_(name) {}

    virtual bool IsInput() { return false; }
    virtual bool IsOutput() { return false; }

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

template <typename T, typename TBase> class ConnectorT : public TBase {
public:
    template <typename... Args>
    ConnectorT(Node &node, std::string name, Args &&...args)
        : TBase(node, name, std::forward<Args>(args)...) {}

    T Read() { return value_; }
    virtual void Write(T value) { value_ = value; }
    T value_;
};

// Forward declarations
template <typename T, typename TBase = Connector> class InputT;
template <typename T, typename TBase = Connector> class OutputT;

template <typename T, typename TBase>
class OutputT : public ConnectorT<T, TBase> {
    using TCallback = std::function<void(T)>;
    using TConnection = ConnectionT<T>;
    using TInput = InputT<T, TBase>;

public:
    template <typename... Args>
    OutputT(Node &node, std::string name, Args &&...args)
        : ConnectorT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    bool IsOutput() override { return true; }

    Connection *Connect(Connector &_input) override {
        // TODO: Only allow connecting to InputT<T>
        return nullptr;
    }

    void Disconnect(Connection &connection) override {
        Unsubscribe(connection);
    }

    void Publish(T value) {
        for (auto connection : connections_) {
            connection->callback_(value);
        }
    }

    TConnection *Subscribe(Connector &input, const TCallback &callback) {
        TConnection *connection = new TConnection(*this, input, callback);
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

    virtual void Write(T value) {
        ConnectorT<T, TBase>::Write(value);
        Publish(value);
    }

    std::list<TConnection *> connections_;
};

template <typename T, typename TBase>
class InputT : public ConnectorT<T, TBase> {
    using TConnection = ConnectionT<T>;
    using TOutput = OutputT<T, TBase>;

public:
    template <typename... Args>
    InputT(Node &node, std::string name, Args &&...args)
        : ConnectorT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    bool IsInput() override { return true; }

    Connection *Connect(Connector &_output) override {
        TOutput *output = dynamic_cast<TOutput *>(&_output);
        if (!output)
            return nullptr;
        return output->Subscribe(*this,
                                 [this](T value) { this->Write(value); });
    }

    void Disconnect(Connection &connection) override {
        connection.output_->Disconnect(connection);
    }
};

} // namespace augr