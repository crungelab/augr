# Phasing
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/physicalModeling/faust-stk/blowBottle.dsp"
  "BlowBottleDsp"
  "blow_bottle_dsp.h"
)
#list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

target_sources(${THIS}
  PRIVATE
  faust-stk.cpp
)