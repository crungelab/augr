#include <augr/core/archive.h>
#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>
#include <augr/core/model_factory.h>

#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/module/module.h>

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

class MyApp : public App {
public:
    MyApp() {
        BubbleDsp &m = ModelFactoryT<BubbleDspImpl>::Make(rack_);
        rack_.AddModule(m);
        bubble_ = &m;

        view_ = new RackView(rack_);
    }

    void Draw() override {
        view_->Draw();
        App::Draw();
    }
    // Data members
    ExeRack rack_;
    RackView *view_;
    BubbleDsp *bubble_;
};

void SerializeBubble(Module &module) {
    auto &manufacturer = ArchiverManufacturer::singleton();

    // Look up the archiver factory for this module's dynamic type.
    auto *factory = manufacturer.FindFactory(std::type_index(typeid(module)));
    if (!factory) {
        std::cerr << "No archiver factory registered for type "
                  << typeid(module).name() << "\n";
        return;
    }

    // Produce an archiver bound to this module.
    auto archiver = factory->Produce(module);
    if (!archiver) {
        std::cerr << "Failed to construct archiver\n";
        return;
    }

    // Build the JSON object the archiver will populate.
    nlohmann::json j;
    Archive archive(j);

    archiver->Save(archive);

    // Pretty-print to stdout.
    std::cout << j.dump(2) << "\n";
}

int main(int, char **) {
    REGISTER_ARCHIVER_FACTORY(BubbleDspArchiver);

    MyApp &app = *new MyApp();
    // ExeRack &rack = app.rack_;
    // rack.Create();
    // rack.Start();

    // Serialize the bubble module before entering the run loop.
    SerializeBubble(*app.bubble_);

    // app.Run(augr::Window::RunParams("Augr Bubble"));
    // rack.Stop();

    return 0;
}