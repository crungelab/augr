#pragma once

#include <string>
#include <vector>
#include <list>
#include <functional>

#include <augr/core/part.h>

namespace augr {

class Node;
class Wire;

class Subscription
{
};

template <typename T = void>
class SubscriptionT : public Subscription
{
public:
  typedef std::function<void(T)> TCallback;

  SubscriptionT(const TCallback &callback) : callback_(callback)
  {
  }
  TCallback callback_;
};

class Pin : public Part
{
public:
  Pin(Node &node, std::string name) : name_(name) {}
  virtual void AddWire(Wire &wire) { wires_.push_back(&wire); }
  virtual Subscription *Connect(Pin &input) = 0;
  // Data members
  Node &node() { return *(Node *)owner_; }
  std::string name_;
  std::vector<Wire *> wires_;
  //REFLECT_ENABLE()
};

template <typename T>
class PinT : public Pin
{
  typedef std::function<void(T)> TCallback;
  typedef SubscriptionT<T> TSubscription;

public:
  PinT(Node &node, std::string name) : Pin(node, name) {}
  Subscription *Connect(Pin &_input) override
  {
    PinT<T> *input = dynamic_cast<PinT<T> *>(&_input);
    return Subscribe([input](T value)
                     { input->Write(value); });
  }
  virtual void Write(T value)
  {
    value_ = value;
    Publish(value);
  }
  T Read() { return value_; }
  // Subscriptions
  std::list<TSubscription *> subscriptions_;
  void Publish(T value)
  {
    for (auto subscription : subscriptions_)
    {
      subscription->callback_(value);
    }
  }
  TSubscription *Subscribe(const TCallback &callback)
  {
    TSubscription *subscription = new SubscriptionT<T>(callback);
    subscriptions_.push_back(subscription);
    return subscription;
  }
  // Accessors
  // Data members
  T value_;
};

} // namespace augr