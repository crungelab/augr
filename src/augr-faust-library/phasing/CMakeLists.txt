# Phasing
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/phasing/phaser.dsp"
  "PhaserDsp"
  "phaser_dsp.h"
)

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/phasing/flanger.dsp"
  "FlangerDsp"
  "flanger_dsp.h"
)

target_sources(${THIS}
  PRIVATE
  phasing.cpp
)