# Phasing
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/physicalModeling/faust-stk/blowBottle.dsp"
  "BlowBottleDsp"
  "blow_bottle_dsp.h"
)

target_sources(${THIS}
  PRIVATE
  faust-stk.cpp
)