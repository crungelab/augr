#include "implot.h"

#include "implot_system.h"


void ImPlotSystem::Create() {
  ImPlot::CreateContext();
}
void ImPlotSystem::Destroy() {
  ImPlot::DestroyContext();
}