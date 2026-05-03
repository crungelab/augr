#include <augr/exe/rack/exe_rack.h>
#include <augr/rack/module/module.h>
#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augite/app/app.h>

#include <augite/widget/widget.h>
#include <augite/widget/widget_manufacturer.h>
#include <augite/view/rack_view.h>

#include "osc_dsp.h"
#include "freeverb_dsp.h"

using namespace augr;

class OscDspImpl : public OscDsp
{
  REFLECT_ENABLE(FaustDsp)
};

class FreeVerbDspImpl : public FreeVerbDsp
{
  REFLECT_ENABLE(FaustDsp)
};

class MyApp : public App
{
public:
  MyApp()
  {
    OscDsp &m = ModelFactoryT<OscDspImpl>::Make(rack_);
    FreeVerbDsp &m2 = ModelFactoryT<FreeVerbDspImpl>::Make(rack_);
    rack_.AddModule(m);
    rack_.AddModule(m2);
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
  app.Run();
  rack.Stop();
}
