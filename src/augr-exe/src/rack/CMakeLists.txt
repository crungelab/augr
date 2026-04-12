cmake_minimum_required(VERSION 3.14)

include(${AUGR_CMAKE}/AugrExe.cmake)

set(RACK_SRC 
  exe_rack.cpp
  audio_system.cpp
  midi_system.cpp
)

target_sources(${THIS}
  PRIVATE
  ${RACK_SRC}
)

INTERNAL_AUGR_EXE(${THIS})
