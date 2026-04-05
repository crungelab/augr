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

// Typed connection — also owns the input pin slot
template <typename T> class ConnectionT : public Connection {
public:
    using TCallback = std::function<void(T)>;

    ConnectionT(Connector &output, Connector &input, PinT<T> *pin_slot,
                const TCallback &callback)
        : Connection(output, input), pin_slot_(pin_slot), callback_(callback) {}

    PinT<T> *pin_slot_ =
        nullptr; // owned by InputT, identified here for removal
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
        : TBase(node, name, std::forward<Args>(args)...),
          pin_(std::make_unique<PinT<T>>()) {}

    T Read() const { return pin_->Read(); }
    virtual void Write(T value) { pin_->Write(value); }

    std::unique_ptr<PinT<T>> pin_;
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

    TConnection *Subscribe(Connector &input, PinT<T> *pin_slot,
                           const TCallback &callback) {
        auto *connection = new TConnection(*this, input, pin_slot, callback);
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

// Input connector — accumulates into per-connection pin slots, mixes on Read
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

        // Allocate a dedicated pin slot for this connection
        auto *slot = new PinT<T>();
        slots_.push_back(slot);

        return output->Subscribe(*this, slot, [this, slot](T value) {
            slot->Write(value);
            dirty_ = true;
        });
    }

    void Disconnect(Connection &connection) override {
        auto *typed = dynamic_cast<TConnection *>(&connection);
        if (typed) {
            // Remove and delete the pin slot owned by this connection
            auto it = std::find(slots_.begin(), slots_.end(), typed->pin_slot_);
            if (it != slots_.end()) {
                delete *it;
                slots_.erase(it);
            }
        }
        // Delegate unsubscribe to the output
        connection.output_->Disconnect(connection);

        // If no connections remain, clear cached value and dirty flag
        if (slots_.empty()) {
            this->pin_->Write(T{});
            dirty_ = false;
        }
    }

    T Read() {
        if (dirty_) {
            if (slots_.size() == 1) {
                this->pin_->Write(slots_[0]->Read());
            } else {
                T mixed = slots_[0]->Read();
                for (size_t i = 1; i < slots_.size(); ++i)
                    mixed += slots_[i]->Read();
                this->pin_->Write(mixed);
            }
            dirty_ = false;
        }
        return this->pin_->Read();
    }

    /*
    T Read() {
        if (dirty_) {
            T mixed = std::accumulate(
                slots_.begin(), slots_.end(), T{},
                [](T acc, const PinT<T> *slot) { return acc + slot->Read(); }
            );
            this->pin_->Write(mixed);
            dirty_ = false;
        }
        return this->pin_->Read();
    }
    */

    // For simplicity, just return the most recent value (no mixing)

    ~InputT() {
        for (auto *slot : slots_)
            delete slot;
    }

    std::vector<PinT<T> *> slots_;
    bool dirty_ = false;
};

} // namespace augr