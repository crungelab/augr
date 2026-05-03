#include <augr/exe/rack/exe_rack.h>
#include <augr/rack/module/module.h>
#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augite/app/app.h>

#include <augite/view/rack_view.h>
#include <augite/widget/widget.h>
#include <augite/widget/widget_manufacturer.h>

#include "chain_dsp.h"

using namespace augr;

class ChainDspImpl : public ChainDsp
{
  REFLECT_ENABLE(FaustDsp)
};

class MyApp : public App
{
public:
  MyApp()
  {
    ChainDsp &m = ModelFactoryT<ChainDspImpl>::Make(rack_);
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
  app.Run();
  rack.Stop();
}
