#pragma once

#include "viewer.h"

namespace augr {

// Factory for Viewer instances. Keyed on the model type so the
// UI builder can look up the right viewer to produce for a given
// model at runtime.
class ViewerFactory {
public:
    virtual ~ViewerFactory() = default;
    // Returns an owned widget. Caller is responsible for inserting it
    // into the widget tree (typically via parent->AddChild(...)).
    virtual Viewer::Ptr Produce(const std::string &label, Document &doc,
                                Model &model) = 0;
    virtual std::type_index GetKey() = 0;

    // Accessors
    const std::string &name() const { return name_; }
    // Data members
private:
    std::string name_;
};

template <typename T, typename D = Document, typename N = Model>
class ViewerFactoryT : public ViewerFactory {
    Viewer::Ptr Produce(const std::string &label, Document &doc,
                        Model &model) override {
        return std::make_unique<T>(label, static_cast<D &>(doc), static_cast<N &>(model));
    }
    std::type_index GetKey() override { return std::type_index(typeid(N)); }
};

#define DEFINE_VIEWER_FACTORY(T, D, N)                                            \
    ViewerFactoryT<T, D, N> T##Factory;                                           \
    ViewerFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr