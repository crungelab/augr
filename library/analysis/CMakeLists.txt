# Analysis
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/analysis/dbmeter.dsp"
  "DbMeterDsp"
  "dbmeter_dsp.h"
)

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/analysis/vumeter.dsp"
  "VuMeterDsp"
  "vumeter_dsp.h"
)

target_sources(${THIS}
  PRIVATE
  analysis.cpp
)