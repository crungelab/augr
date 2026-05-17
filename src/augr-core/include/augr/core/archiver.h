#pragma once

#include <nlohmann/json.hpp>

#include <typeindex>
#include <utility>
#include <vector>

#include "archive.h"

namespace augr {

class ArchiverFactory;
class Subject;

// Archiver is instantiated per Model. Each Model has its own archiver,
// produced by an ArchiverFactory and bound to that Model at construction.
// Per-instance state (caches, intermediate values, references back to
// the Model) lives here.
class Archiver {
public:
    virtual ~Archiver() = default;
    virtual void Create(ArchiverFactory &factory, Subject &model) = 0;
    virtual void Save(Archive &archive) const = 0;
    virtual void Load(Archive &archive) = 0;

protected:
    Subject *subject_ = nullptr;
    ArchiverFactory *factory_ = nullptr;
};

// ArchiverT — typed convenience layer. Subclasses of ArchiverT<T>
// see their Model as T& without writing static_casts.
template <typename T, typename TBase = Archiver> class ArchiverT : public TBase {
public:
    void Create(ArchiverFactory &factory, Subject &subject) override {
        this->factory_ = &factory;
        this->subject_ = &dynamic_cast<T &>(subject);
    }
    T &subject() { return *static_cast<T *>(this->subject_); }
    const T &subject() const { return *static_cast<T *>(this->subject_); }
};

} // namespace augr