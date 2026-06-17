#include "implot.h"

#include <augite/system/implot_system.h>


void ImPlotSystem::Create() {
  ImPlot::CreateContext();
}
void ImPlotSystem::Destroy() {
  ImPlot::DestroyContext();
}