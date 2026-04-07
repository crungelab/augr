#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <augr/core/part.h>
#include <augr/core/rack/slot.h>

namespace augr {

class Node;
class Wire;
class Pin;

// Base connection — holds pin-level pointers
class Connection {
public:
    Connection(Pin &output, Pin &input)
        : output_(&output), input_(&input) {}
    virtual ~Connection() = default;

    Pin *output_;
    Pin *input_;
};

// Typed connection — also owns the input slot
template <typename T> class ConnectionT : public Connection {
public:
    using TCallback = std::function<void(T)>;

    ConnectionT(Pin &output, Pin &input, SlotT<T> *slot,
                const TCallback &callback)
        : Connection(output, input), slot_(slot), callback_(callback) {}

    SlotT<T> *slot_ = nullptr; // owned by InputT, identified here for removal
    TCallback callback_;
};

// Base pin — wiring, name, node ref
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

// Typed pin base — holds the primary slot
template <typename T, typename TBase = Pin>
class PinT : public TBase {
public:
    template <typename... Args>
    PinT(Node &node, std::string name, Args &&...args)
        : TBase(node, name, std::forward<Args>(args)...) {}

    T Read() const { return value_; }
    virtual void Write(T value) { value_ = value; }

    T value_{};
};

// Forward declarations
template <typename T, typename TBase = Pin> class InputT;
template <typename T, typename TBase = Pin> class OutputT;

// Output pin — publishes to all subscribers
template <typename T, typename TBase>
class OutputT : public PinT<T, TBase> {
    using TCallback = std::function<void(T)>;
    using TConnection = ConnectionT<T>;
    using TInput = InputT<T, TBase>;

public:
    template <typename... Args>
    OutputT(Node &node, std::string name, Args &&...args)
        : PinT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    Connection *Connect(Pin &input) override {
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

    TConnection *Subscribe(Pin &input, SlotT<T> *slot,
                           const TCallback &callback) {
        auto *connection = new TConnection(*this, input, slot, callback);
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
        PinT<T, TBase>::Write(value);
        Publish(value);
    }

    std::list<TConnection *> connections_;
};

// Input pin — uses per-connection slots, mixes on Read
template <typename T, typename TBase>
class InputT : public PinT<T, TBase> {
    using TConnection = ConnectionT<T>;
    using TOutput = OutputT<T, TBase>;

public:
    template <typename... Args>
    InputT(Node &node, std::string name, Args &&...args)
        : PinT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    Connection *Connect(Pin &_output) override {
        TOutput *output = dynamic_cast<TOutput *>(&_output);
        if (!output)
            return nullptr;

        // Allocate a dedicated slot for this connection
        auto *slot = new SlotT<T>();
        slots_.push_back(slot);

        return output->Subscribe(*this, slot, [this, slot](T value) {
            slot->Write(value);
            dirty_ = true;
        });
    }

    void Disconnect(Connection &connection) override {
        auto *typed = dynamic_cast<TConnection *>(&connection);
        if (typed) {
            // Remove and delete the slot owned by this connection
            auto it = std::find(slots_.begin(), slots_.end(), typed->slot_);
            if (it != slots_.end()) {
                delete *it;
                slots_.erase(it);
            }
        }
        // Delegate unsubscribe to the output
        connection.output_->Disconnect(connection);

        // If no connections remain, clear cached value and dirty flag
        if (slots_.empty()) {
            this->value_ = T{};
            dirty_ = false;
        }
    }

    virtual T Reduce() const {
        T mixed = slots_[0]->Read();
        for (size_t i = 1; i < slots_.size(); ++i)
            mixed += slots_[i]->Read();
        return mixed;
    }

    T Read() {
        if (dirty_) {
            this->value_ = (slots_.size() == 1 ? slots_[0]->Read() : Reduce());
            dirty_ = false;
        }
        return this->value_;
    }

    ~InputT() {
        for (auto *slot : slots_)
            delete slot;
    }

    std::vector<SlotT<T> *> slots_;
    bool dirty_ = false;
};

} // namespace augr