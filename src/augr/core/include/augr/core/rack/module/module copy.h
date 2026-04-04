#pragma once

#include <vector>

#include <augr/core/rack/node.h>
#include <augr/core/rack/audio_pin.h>

namespace augr {

class Module : public Node
{
public:
  //
  virtual bool Create(Part &owner) override;
  virtual Audio ProcessAudio(Audio input = Audio()) { return Audio(); }
  virtual void Process() {}
  // Data members
  const char *label_ = nullptr;
  AudioPin *audio_in_ = nullptr;
  AudioPin *audio_out_ = nullptr;

  REFLECT_ENABLE(Node)
};

// Factory

/*
template <typename T>
class ModuleFactoryT : public ModelFactoryT<T> {
public:
    ModuleFactoryT(std::string name, std::string category) : name_(name), category_(category) {}
    // Data members
    std::string name_;
    std::string category_;
};

#define DEFINE_MODULE_FACTORY(T, NAME, CATEGORY)                                                    \
    ModuleFactoryT<T> T##Factory(NAME, CATEGORY);                                           \
    ModelFactory *Get##T##Factory() { return &T##Factory; }
*/

} // namespace augr