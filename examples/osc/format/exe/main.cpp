#include <augr/core/model_factory.h>

#include <augr/rack/module/module.h>
#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/view/rack_view.h>
#include <augite/widget/widget.h>
#include <augite/widget/widget_manufacturer.h>
#include <augite/app/app.h>

#include "osc_dsp.h"

using namespace augr;

class OscDspImpl : public OscDsp
{
  REFLECT_ENABLE(FaustDsp)
};

class MyApp : public App
{
public:
  MyApp()
  {
    OscDsp &m = ModelFactoryT<OscDspImpl>::Make(rack_);
    rack_.AddModule(m);
    view_ = new RackView(rack_);
  }

  void Draw() override
  {
    App::Draw();
    view_->Draw();
  }
  // Data members
  ExeRack rack_;
  RackView *view_;
};

int main(int, char **)
{
  MyApp &app = *new MyApp();
  ExeRack &rack = app.rack_;
  rack.Create();
  rack.Start();
  app.Run(augr::Window::RunParams("Augr Osc"));
  rack.Stop();
}
