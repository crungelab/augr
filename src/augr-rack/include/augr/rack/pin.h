#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <augr/core/part.h>
#include <augr/rack/slot.h>

namespace augr {

class Node;
class Wire;
class Pin;

// Base connection — holds pin-level pointers
class Connection {
public:
    Connection(Pin &output, Pin &input) : output_(&output), input_(&input) {}
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

enum class PinKind {
    Output,
    MonoInput,
    QueueInput,
    PolyInput,
};

// Base pin — wiring, name, node ref
class Pin : public Part {
public:
    Pin(Node &node, std::string name) : name_(name) {}

    virtual PinKind kind() const = 0;

    bool is_input() const { return kind() != PinKind::Output; }
    bool is_output() const { return kind() == PinKind::Output; }

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

// Typed pin base — holds the primary value
template <typename T, typename TBase = Pin> class PinT : public TBase {
public:
    template <typename... Args>
    PinT(Node &node, std::string name, Args &&...args)
        : TBase(node, name, std::forward<Args>(args)...) {}

    T Read() const { return value_; }
    virtual void Write(T value) { value_ = value; }

    // Hook for subclasses to coerce incoming values (format conversion,
    // clamping, quantization, etc.). Called by input pins inside the
    // delivery callback before the value is stored. Identity by default.
    virtual T Transform(T value) { return value; }

    T value_{};
};

// Forward declarations
template <typename T, typename TBase = Pin> class MonoInputT;
template <typename T, typename TBase = Pin> class PolyInputT;
template <typename T, typename TBase = Pin> class OutputT;

// Output pin — publishes to all subscribers
template <typename T, typename TBase> class OutputT : public PinT<T, TBase> {
    using TCallback = std::function<void(T)>;
    using TConnection = ConnectionT<T>;
    using TMonoInput = MonoInputT<T, TBase>;
    using TPolyInput = PolyInputT<T, TBase>;

public:
    template <typename... Args>
    OutputT(Node &node, std::string name, Args &&...args)
        : PinT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    PinKind kind() const override { return PinKind::Output; }

    Connection *Connect(Pin &input) override {
        // Connection is initiated from the input side
        return nullptr;
    }

    void Disconnect(Connection &connection) override {
        DestroyConnection(connection);
    }

    void Publish(T value) {
        for (auto *conn : connections_) {
            conn->callback_(value);
        }
    }

    TConnection *CreateConnection(Pin &input, SlotT<T> *slot,
                                  const TCallback &callback) {
        auto *connection = new TConnection(*this, input, slot, callback);
        connections_.push_back(connection);
        return connection;
    }

    void DestroyConnection(Connection &connection) {
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

// MonoInputT — single-connection input; rejects additional connections
template <typename T, typename TBase> class MonoInputT : public PinT<T, TBase> {
    using TConnection = ConnectionT<T>;
    using TOutput = OutputT<T, TBase>;

public:
    template <typename... Args>
    MonoInputT(Node &node, std::string name, Args &&...args)
        : PinT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    PinKind kind() const override { return PinKind::MonoInput; }

    Connection *Connect(Pin &_output) override {
        if (connection_)
            return nullptr;

        TOutput *output = dynamic_cast<TOutput *>(&_output);
        if (!output)
            return nullptr;

        connection_ = output->CreateConnection(*this, nullptr, [this](T value) {
            this->value_ = this->Transform(value);
        });

        return connection_;
    }

    void Disconnect(Connection &connection) override {
        if (&connection != connection_)
            return;

        connection.output_->Disconnect(connection);
        connection_ = nullptr;

        this->value_ = T{};
    }

    bool connected() const { return connection_ != nullptr; }

protected:
    TConnection *connection_ = nullptr;
};

// QueueInputT — single-connection input that accumulates incoming values
template <typename T, typename TBase>
class QueueInputT : public MonoInputT<T, TBase> {
    using TConnection = ConnectionT<T>;
    using TOutput = OutputT<T, TBase>;

public:
    template <typename... Args>
    QueueInputT(Node &node, std::string name, Args &&...args)
        : MonoInputT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    PinKind kind() const override { return PinKind::QueueInput; }

    Connection *Connect(Pin &_output) override {
        if (this->connection_)
            return nullptr;

        TOutput *output = dynamic_cast<TOutput *>(&_output);
        if (!output)
            return nullptr;

        this->connection_ =
            output->CreateConnection(*this, nullptr, [this](T value) {
                std::lock_guard<std::mutex> lock(mutex_);
                queue_.push_back(this->Transform(value));
            });
        return this->connection_;
    }

    std::vector<T> Drain() {
        std::vector<T> out;
        std::lock_guard<std::mutex> lock(mutex_);
        out.swap(queue_);
        return out;
    }

    bool Empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::vector<T> queue_;
    mutable std::mutex mutex_;
};

// PolyInputT — multi-connection input; mixes all sources via Reduce()
template <typename T, typename TBase> class PolyInputT : public PinT<T, TBase> {
    using TConnection = ConnectionT<T>;
    using TOutput = OutputT<T, TBase>;

public:
    template <typename... Args>
    PolyInputT(Node &node, std::string name, Args &&...args)
        : PinT<T, TBase>(node, name, std::forward<Args>(args)...) {}

    PinKind kind() const override { return PinKind::PolyInput; }

    Connection *Connect(Pin &_output) override {
        TOutput *output = dynamic_cast<TOutput *>(&_output);
        if (!output)
            return nullptr;

        auto *slot = new SlotT<T>();
        slots_.push_back(slot);

        return output->CreateConnection(*this, slot, [this, slot](T value) {
            slot->Write(this->Transform(value));
            dirty_ = true;
        });
    }

    void Disconnect(Connection &connection) override {
        auto *typed = dynamic_cast<TConnection *>(&connection);
        if (typed) {
            auto it = std::find(slots_.begin(), slots_.end(), typed->slot_);
            if (it != slots_.end()) {
                delete *it;
                slots_.erase(it);
            }
        }
        connection.output_->Disconnect(connection);

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

    size_t connection_count() const { return slots_.size(); }

    ~PolyInputT() {
        for (auto *slot : slots_)
            delete slot;
    }

    std::vector<SlotT<T> *> slots_;

protected:
    bool dirty_ = false;
};

} // namespace augr