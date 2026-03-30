# Game Audio
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/gameaudio/rain.dsp"
  "RainDsp"
  "rain_dsp.h"
)
#list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/gameaudio/thunder.dsp"
  "ThunderDsp"
  "thunder_dsp.h"
)
#list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

target_sources(${THIS}
  PRIVATE
  gameaudio.cpp
)