#include "spectral_widget.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include <implot.h>
#include <kiss_fftr.h>

#include <augr/rack/library/spectral_module.h>

namespace augr {

DEFINE_MODEL_WIDGET_FACTORY(SpectralWidget, SpectralModule)

} // namespace augr