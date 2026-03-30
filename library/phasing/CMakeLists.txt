# Phasing
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/phasing/phaser.dsp"
  "PhaserDsp"
  "phaser_dsp.h"
)
#list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/phasing/flanger.dsp"
  "FlangerDsp"
  "flanger_dsp.h"
)
#list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

target_sources(${THIS}
  PRIVATE
  phasing.cpp
)