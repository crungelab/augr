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
DEFINE_MODEL_FACTORY(BubbleDspImpl, "BubbleDsp", "Faust")

class BubbleDspArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(BubbleDspArchiver, BubbleDspImpl, "BubbleDsp")

class ExeRackArchiver : public RackArchiver {};
//DEFINE_ARCHIVER_FACTORY(ExeRackArchiver, ExeRack, "ExeRack")
DEFINE_ARCHIVER_FACTORY(ExeRackArchiver, ExeRack, "Rack")

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

//DEFINE_MODEL_FACTORY(ExeRack, "ExeRack", "Rack")
DEFINE_MODEL_FACTORY(ExeRack, "Rack", "Rack")
DEFINE_MODEL_FACTORY(AudioInputDevice, "AudioInputDevice", "Rack")
DEFINE_MODEL_FACTORY(AudioOutputDevice, "AudioOutputDevice", "Rack")
DEFINE_MODEL_FACTORY(MidiInputDevice, "MidiInputDevice", "Rack")
DEFINE_MODEL_FACTORY(MidiOutputDevice, "MidiOutputDevice", "Rack")


class MyApp : public App {
public:
    MyApp() {
        rack_ = new ExeRack();
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
};

// Serialize a rack to JSON and print it.
nlohmann::json SerializeRack(ExeRack &rack) {
    auto &manufacturer = ArchiverManufacturer::singleton();
    auto *factory = manufacturer.FindFactory(std::type_index(typeid(rack)));
    if (!factory) {
        std::cerr << "No archiver factory registered for type "
                  << typeid(rack).name() << "\n";
        return nlohmann::json();
    }

    auto *archiver = factory->Produce(rack);
    if (!archiver) {
        std::cerr << "Failed to construct archiver\n";
        return nlohmann::json();
    }

    nlohmann::json j;
    Archive archive(j);
    archiver->Save(archive);
    delete archiver;

    std::cout << j.dump(2) << "\n";
    return j;
}

// Deserialize JSON into a fresh rack constructed as a root (no parent).
ExeRack *DeserializeRack(const nlohmann::json &j) {
    if (!j.contains("type")) {
        std::cerr << "JSON missing 'type' field\n";
        return nullptr;
    }
    std::string type_name = j["type"].get<std::string>();

    // Construct the rack as a root via its model factory.
    auto &model_manufacturer = ModelManufacturer::singleton();
    ModelFactory *model_factory = model_manufacturer.FindFactory(type_name);
    if (!model_factory) {
        std::cerr << "No model factory registered for type '" << type_name
                  << "'\n";
        return nullptr;
    }

    ExeRack *rack = dynamic_cast<ExeRack *>(model_factory->Produce(nullptr));
    if (!rack) {
        std::cerr << "Factory produced a non-ExeRack for type '" << type_name
                  << "'\n";
        return nullptr;
    }

    // Run the archiver to populate state from JSON.
    auto &archiver_manufacturer = ArchiverManufacturer::singleton();
    ArchiverFactory *archiver_factory =
        archiver_manufacturer.FindFactory(type_name);
    if (!archiver_factory) {
        std::cerr << "No archiver factory registered for type '" << type_name
                  << "'\n";
        return rack;
    }

    Archiver *archiver = archiver_factory->Produce(*rack);
    Archive archive(const_cast<nlohmann::json &>(j));
    archiver->Load(archive);
    delete archiver;

    return rack;
}

int main(int, char **) {
    REGISTER_MODEL_FACTORY(ExeRack);
    REGISTER_MODEL_FACTORY(AudioInputDevice);
    REGISTER_MODEL_FACTORY(AudioOutputDevice);
    REGISTER_MODEL_FACTORY(MidiInputDevice);
    REGISTER_MODEL_FACTORY(MidiOutputDevice);
    REGISTER_MODEL_FACTORY(BubbleDspImpl);

    REGISTER_ARCHIVER_FACTORY(BubbleDspArchiver);
    REGISTER_ARCHIVER_FACTORY(ExeRackArchiver);
    REGISTER_ARCHIVER_FACTORY(AudioInputDeviceArchiver);
    REGISTER_ARCHIVER_FACTORY(AudioOutputDeviceArchiver);
    REGISTER_ARCHIVER_FACTORY(MidiInputDeviceArchiver);
    REGISTER_ARCHIVER_FACTORY(MidiOutputDeviceArchiver);

    MyApp *app = new MyApp();
    auto &rack = app->rack();
    rack.Create();
    rack.CreateDefaultDevices();

    auto bubble = ModelFactoryT<BubbleDspImpl>::Make(&rack);

    rack.Connect(*bubble->audio_out_,
                 *rack.audio_output_device_->audio_in_);

    // Round-trip test: serialize the rack, deserialize into a fresh
    // rack, re-serialize. The two outputs should match if save/load
    // is consistent.
    std::cout << "=== Original rack ===\n";
    nlohmann::json original_json = SerializeRack(rack);

    ExeRack *reloaded = DeserializeRack(original_json);
    if (reloaded) {
        std::cout << "\n=== Reloaded rack ===\n";
        nlohmann::json reloaded_json = SerializeRack(*reloaded);

        std::cout << "\nRound-trip match: "
                  << (original_json == reloaded_json ? "yes" : "no") << "\n";

        delete reloaded;
    }

    app->Run(augr::Window::RunParams("Augr Bubble"));

    return 0;
}