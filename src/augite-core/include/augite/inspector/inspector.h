#pragma once

#include "../widget/widget.h"

namespace augr {

/*
class Inspector : public ModelWidget {
public:
    Inspector() = default;
    virtual ~Inspector() = default;
};

template <typename T, typename TBase = Inspector>
class InspectorT : public TBase {
public:
    InspectorT(T &model) : TBase(model) {}
    T &model() { return *static_cast<T *>(TBase::model_); }
    const T &model() const { return *static_cast<const T *>(TBase::model_); }
};
*/

} // namespace augr

/*
#define DEFINE_INSPECTOR_FACTORY(T, N)                                      \
    ModelWidgetFactoryT<T, N> T##Factory;                                      \
    ModelWidgetFactory *Get##T##Factory() { return &T##Factory; }
*/