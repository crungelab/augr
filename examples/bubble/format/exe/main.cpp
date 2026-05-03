#include <augr/exe/rack/exe_rack.h>
#include <augr/rack/module/module.h>
#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augite/app/app.h>

#include <augite/widget/widget.h>
#include <augite/widget/widget_manufacturer.h>
#include <augite/view/rack_view.h>

#include "bubble_dsp.h"

using namespace augr;

class BubbleDspImpl : public BubbleDsp
{
public:
  REFLECT_ENABLE(BubbleDsp)
};

class MyApp : public App
{
public:
  MyApp()
  {
    BubbleDsp &m = ModelFactoryT<BubbleDspImpl>::Make(rack_);
    rack_.AddModule(m);

    view_ = new RackView(rack_);
  }

  void Draw() override
  {
    view_->Draw();
    App::Draw();
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
  app.Run(augr::Window::RunParams("Augr Bubble"));
  rack.Stop();

  return 0;
}
