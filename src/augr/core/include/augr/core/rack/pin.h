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

class Subscription {
public:
    Subscription(const Pin &pin) : pin_(&pin) {}
    const Pin *pin_;
};

template <typename T = void> class SubscriptionT : public Subscription {
public:
    typedef std::function<void(T)> TCallback;

    SubscriptionT(const Pin &pin, const TCallback &callback) : Subscription(pin), callback_(callback) {}
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
    virtual Subscription *Connect(Pin &input) = 0;
    virtual void Disconnect(Pin &input) = 0;
    //virtual void Unsubscribe(Subscription *subscription) = 0;
    // Data members
    Node &node() { return *(Node *)owner_; }
    std::string name_;
    std::vector<Wire *> wires_;
};

template <typename T> class PinT : public Pin {
    typedef std::function<void(T)> TCallback;
    typedef SubscriptionT<T> TSubscription;

public:
    PinT(Node &node, std::string name) : Pin(node, name) {}
    Subscription *Connect(Pin &_input) override {
        PinT<T> *input = dynamic_cast<PinT<T> *>(&_input);
        return Subscribe(*input, [input](T value) { input->Write(value); });
    }
    void Disconnect(Pin &input) override {
        // Unsubscribe logic here (requires tracking subscriptions)
        PinT<T> *input_pin = dynamic_cast<PinT<T> *>(&input);
        // Find and remove the subscription for this input_pin
        for (auto it = subscriptions_.begin(); it != subscriptions_.end(); ++it) {
            TSubscription *subscription = dynamic_cast<TSubscription *>(*it);
            if (subscription && subscription->pin_ == &input) {
                Unsubscribe(subscription);
                break;
            }
        }
    }
    virtual void Write(T value) {
        value_ = value;
        Publish(value);
    }
    T Read() { return value_; }
    // Subscriptions
    std::list<TSubscription *> subscriptions_;
    void Publish(T value) {
        for (auto subscription : subscriptions_) {
            subscription->callback_(value);
        }
    }
    TSubscription *Subscribe(const Pin& pin, const TCallback &callback) {
        TSubscription *subscription = new SubscriptionT<T>(pin, callback);
        subscriptions_.push_back(subscription);
        return subscription;
    }
    void Unsubscribe(TSubscription *subscription) {
        subscriptions_.remove(subscription);
        delete subscription;
    }
    // Accessors
    // Data members
    T value_;
};

} // namespace augr