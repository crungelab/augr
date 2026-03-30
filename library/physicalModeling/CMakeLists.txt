# Physical Modeling
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/physicalModeling/frenchBell.dsp"
  "FrenchBellDsp"
  "frenchbell_dsp.h"
)
#list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

target_sources(${THIS}
  PRIVATE
  physicalModeling.cpp
)

add_subdirectory(faust-stk)