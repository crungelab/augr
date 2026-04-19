# Generator

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/generator/churchOrgan.dsp"
  "ChurchOrganDsp"
  "church_organ_dsp.h"
)

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/generator/osc.dsp"
  "OscDsp"
  "osc_dsp.h"
)

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/generator/sawtoothLab.dsp"
  "SawtoothLabDsp"
  "sawtooth_lab_dsp.h"
)

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/generator/virtualAnalog.dsp"
  "VirtualAnalogDsp"
  "virtual_analog_dsp.h"
)

target_sources(${THIS}
  PRIVATE
  generator.cpp
)