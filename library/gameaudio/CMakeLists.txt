# Game Audio

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/gameaudio/bubble.dsp"
  "BubbleDsp"
  "bubble_dsp.h"
)

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/gameaudio/rain.dsp"
  "RainDsp"
  "rain_dsp.h"
)

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/gameaudio/thunder.dsp"
  "ThunderDsp"
  "thunder_dsp.h"
)

target_sources(${THIS}
  PRIVATE
  gameaudio.cpp
)