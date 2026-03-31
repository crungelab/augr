cmake_minimum_required(VERSION 3.14)

include(${AUGR_CMAKE}/AugrRtAudio.cmake)

set(RACK_SRC 
  rtaudio_rack.cpp
)

target_sources(${THIS}
  PRIVATE
  ${RACK_SRC}
)

INTERNAL_AUGR_RTAUDIO(${THIS})
