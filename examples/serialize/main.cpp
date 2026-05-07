#include <augr/core/archive.h>
#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>
#include <augr/core/model_manufacturer.h>

#include <augr/rack/archiver/graph_archiver.h>
#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/archiver/rack_archiver.h>
#include <augr/rack/module/module.h>

#include <augr/rack/module/audio_device.h>
#include <augr/rack/module/midi_device.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/app/app.h>
#include <augite/view/rack_view.h>
#include <augite/widget/widget.h>
#include <augite/widget/widget_manufacturer.h>

#include <nlohmann/json.hpp>

#include <iostream>

#include "bubble_dsp.h"

using namespace augr;

class BubbleDspImpl : public BubbleDsp {
public:
    REFLECT_ENABLE(BubbleDsp)
};

class BubbleDspArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(BubbleDspArchiver, BubbleDspImpl, "BubbleDsp")

class ExeRackArchiver : public RackArchiver {};
DEFINE_ARCHIVER_FACTORY(ExeRackArchiver, ExeRack, "ExeRack")

class AudioInputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(AudioInputDeviceArchiver, AudioInputDevice,
                        "AudioInputDevice")

class AudioOutputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(AudioOutputDeviceArchiver, AudioOutputDevice,
                        "AudioOutputDevice")

class MidiInputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(MidiInputDeviceArchiver, MidiInputDevice,
                        "MidiInputDevice")

class MidiOutputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(MidiOutputDeviceArchiver, MidiOutputDevice,
                        "MidiOutputDevice")

class MyApp : public App {
public:
    MyApp() {
        rack_ = new ExeRack();
        BubbleDsp &m = ModelFactoryT<BubbleDspImpl>::Make(rack());
        rack().AddModule(m);
        bubble_ = &m;
        view_ = new RackView(rack());
    }

    void Draw() override {
        view_->Draw();
        App::Draw();
    }
    // Accessors
    ExeRack &rack() { return *rack_; }
    // Data members
    ExeRack *rack_;
    RackView *view_;
    BubbleDsp *bubble_;
};

std::string SerializeModule(Module &module) {
    auto &manufacturer = ArchiverManufacturer::singleton();

    // Look up the archiver factory for this module's dynamic type.
    auto *factory = manufacturer.FindFactory(std::type_index(typeid(module)));
    if (!factory) {
        std::cerr << "No archiver factory registered for type "
                  << typeid(module).name() << "\n";
        return "";
    }

    // Produce an archiver bound to this module.
    auto archiver = factory->Produce(module);
    if (!archiver) {
        std::cerr << "Failed to construct archiver\n";
        return "";
    }

    // Build the JSON object the archiver will populate.
    nlohmann::json j;
    Archive archive(j);

    archiver->Save(archive);

    // Pretty-print to stdout.
    auto result = j.dump(2);
    std::cout << result << "\n";
    return result;
}

Module *DeserializeModule(Graph &parent, const nlohmann::json &j) {
    if (!j.contains("type")) {
        std::cerr << "JSON missing 'type' field\n";
        return nullptr;
    }
    std::string type_name = j["type"].get<std::string>();

    // Construct the Model via its factory. Produce attaches it to the
    // parent graph.
    auto &model_manufacturer = ModelManufacturer::singleton();
    ModelFactory *model_factory = model_manufacturer.FindFactory(type_name);
    if (!model_factory) {
        std::cerr << "No model factory registered for type '" << type_name
                  << "'\n";
        return nullptr;
    }

    Module *module = dynamic_cast<Module *>(model_factory->Produce(parent));
    if (!module) {
        std::cerr << "Factory produced a non-Module for type '" << type_name
                  << "'\n";
        return nullptr;
    }

    // Now find the archiver and have it populate the module from JSON.
    auto &archiver_manufacturer = ArchiverManufacturer::singleton();
    ArchiverFactory *archiver_factory =
        archiver_manufacturer.FindFactory(type_name);
    if (!archiver_factory) {
        std::cerr << "No archiver factory registered for type '" << type_name
                  << "'\n";
        return module; // Module exists but won't have its state loaded.
    }

    Archiver *archiver = archiver_factory->Produce(*module);
    if (!archiver) {
        std::cerr << "Failed to construct archiver for type '" << type_name
                  << "'\n";
        return module;
    }

    // Build the Archive over the JSON. const_cast because Archive's
    // json stack stores non-const pointers — load reads but the API
    // is shared with save.
    Archive archive(const_cast<nlohmann::json &>(j));
    archiver->Load(archive);
    delete archiver;

    return module;
}

int main(int, char **) {
    REGISTER_ARCHIVER_FACTORY(BubbleDspArchiver);
    REGISTER_ARCHIVER_FACTORY(ExeRackArchiver);
    REGISTER_ARCHIVER_FACTORY(AudioInputDeviceArchiver);
    REGISTER_ARCHIVER_FACTORY(AudioOutputDeviceArchiver);
    REGISTER_ARCHIVER_FACTORY(MidiInputDeviceArchiver);
    REGISTER_ARCHIVER_FACTORY(MidiOutputDeviceArchiver);

    MyApp app = *new MyApp();
    auto &rack = app.rack();
    rack.Create();

    rack.Connect(*app.bubble_->audio_out_,
                 *rack.audio_output_device_->audio_in_);

    // rack.Start();

    // Serialize the bubble module before entering the run loop.
    // SerializeModule(*app.bubble_);
    auto json = SerializeModule(rack);
    app.rack_ = (ExeRack*)(DeserializeModule(rack, json));

    app.Run(augr::Window::RunParams("Augr Bubble"));
    // rack.Stop();

    return 0;
}