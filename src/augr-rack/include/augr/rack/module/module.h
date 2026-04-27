#pragma once

#include <memory>
#include <vector>

#include <augr/core/ui/control/float_parameter.h>
#include <augr/core/ui/control/enum_parameter.h>

#include <augr/rack/audio_pin.h>
#include <augr/rack/midi_pin.h>
#include <augr/rack/node.h>

namespace augr {

class Module : public Node {
public:
    bool Create(Part &owner) override;
    virtual void CreatePins() {}
    virtual void CreateControls() {}

    virtual Audio ProcessAudio(Audio &input) { return Audio(); }
    virtual void Process() {}

    // -- Parameters -------------------------------------------------
    FloatParameter *CreateFloatParameter(const std::string &label,
                                         const ControlMeta &meta, float *zone,
                                         const fy_real init, const fy_real min,
                                         const fy_real max,
                                         const fy_real step) {

        auto binding = MakePointerBinding(zone);
        auto param = FloatParameter::Make(label, meta, std::move(binding), init,
                                          min, max, step);
        FloatParameter *raw = param.get();
        AddParameter(std::move(param));
        return raw;
    }

    template <typename T>
    EnumParameterT<T> *CreateEnumParameter(
        const std::string &label, const ControlMeta &meta, T *zone,
        std::vector<typename EnumParameterT<T>::Choice> choices, T init) {

        auto binding = MakePointerBinding(zone);
        auto param = std::make_unique<EnumParameterT<T>>(
            label, meta, std::move(binding), std::move(choices), init);
        EnumParameterT<T> *raw = param.get();
        AddParameter(std::move(param));
        return raw;
    }

    void AddParameter(std::unique_ptr<Parameter> param) {
        parameters_.push_back(std::move(param));
    }

    Parameter *FindParameter(const std::string &label) const {
        for (auto &p : parameters_)
            if (p->label() == label)
                return p.get();
        return nullptr;
    }

    const std::vector<std::unique_ptr<Parameter>> &parameters() const {
        return parameters_;
    }

    // Data members
    const char *label_ = nullptr;
    AudioInput *audio_in_ = nullptr;
    AudioOutput *audio_out_ = nullptr;
    MidiInput *midi_in_ = nullptr;
    MidiOutput *midi_out_ = nullptr;

    REFLECT_ENABLE(Node)

private:
    std::vector<std::unique_ptr<Parameter>> parameters_;
};

} // namespace augr