#pragma once

#include <nlohmann/json.hpp>

#include <typeindex>
#include <utility>
#include <vector>

#include "archive.h"

namespace augr {

class Model;

// Archiver is instantiated per Model. Each Model has its own archiver,
// produced by an ArchiverFactory and bound to that Model at construction.
// Per-instance state (caches, intermediate values, references back to
// the Model) lives here.
class Archiver {
public:
    virtual ~Archiver() = default;

    virtual void Save(Archive &archive) const = 0;
    virtual void Load(Archive &archive) = 0;

    // Accessors
    virtual Model *model_ptr() = 0;
};

// ArchiverT — typed convenience layer. Subclasses of ArchiverT<T>
// see their Model as T& without writing static_casts.
template <typename T> class ArchiverT : public Archiver {
public:
    ArchiverT(T &model) : model_(&model) {}
    Model *model_ptr() override { return model_; }
    T &model() { return *model_; }
    const T &model() const { return *model_; }
    // Data members
    T *model_;
};

} // namespace augr