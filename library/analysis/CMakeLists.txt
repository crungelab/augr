# Analysis
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/analysis/dbmeter.dsp"
  "DbMeterDsp"
  "dbmeter_dsp.h"
)
#list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/analysis/vumeter.dsp"
  "VuMeterDsp"
  "vumeter_dsp.h"
)
#list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

target_sources(${THIS}
  PRIVATE
  analysis.cpp
)