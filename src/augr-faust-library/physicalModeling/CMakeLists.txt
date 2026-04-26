# Physical Modeling
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/physicalModeling/frenchBell.dsp"
  "FrenchBellDsp"
  "frenchbell_dsp.h"
)

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/physicalModeling/brassMIDI.dsp"
  "BrassMidiDsp"
  "brass_midi_dsp.h"
)

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/physicalModeling/clarinetMIDI.dsp"
  "ClarinetMidiDsp"
  "clarinet_midi_dsp.h"
)


target_sources(${THIS}
  PRIVATE
  physicalModeling.cpp
)

add_subdirectory(faust-stk)