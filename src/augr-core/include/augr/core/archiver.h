#pragma once

#include <nlohmann/json.hpp>

#include <typeindex>
#include <utility>
#include <vector>

#include "archive.h"

namespace augr {

class ArchiverFactory;
class Model;

// Archiver is instantiated per Model. Each Model has its own archiver,
// produced by an ArchiverFactory and bound to that Model at construction.
// Per-instance state (caches, intermediate values, references back to
// the Model) lives here.
class Archiver {
public:
    virtual ~Archiver() = default;
    virtual void Create(ArchiverFactory &factory, Model &model) = 0;
    virtual void Save(Archive &archive) const = 0;
    virtual void Load(Archive &archive) = 0;

    // Accessors
    //virtual Model *model_ptr() = 0;

protected:
    Model *model_ = nullptr;
    ArchiverFactory *factory_ = nullptr;
};

// ArchiverT — typed convenience layer. Subclasses of ArchiverT<T>
// see their Model as T& without writing static_casts.
template <typename T, typename TBase = Archiver> class ArchiverT : public TBase {
public:
    void Create(ArchiverFactory &factory, Model &model) override {
        this->factory_ = &factory;
        this->model_ = &dynamic_cast<T &>(model);
    }
    //Model *model_ptr() override { return model_; }
    T &model() { return *(T*)this->model_; }
    const T &model() const { return *(T*)this->model_; }
    // Data members
    //T *model_;
};

} // namespace augr