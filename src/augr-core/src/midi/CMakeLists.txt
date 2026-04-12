cmake_minimum_required(VERSION 3.14)

include(${AUGR_CMAKE}/AugrCore.cmake)

file(GLOB MIDI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

target_sources(${THIS}
  PRIVATE
  ${MIDI_SRC}
)
INTERNAL_AUGR_CORE(${THIS})
