# Physical Modeling
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/physicalModeling/frenchBell.dsp"
  "FrenchBellDsp"
  "frenchbell_dsp.h"
)

target_sources(${THIS}
  PRIVATE
  physicalModeling.cpp
)

add_subdirectory(faust-stk)