cmake_minimum_required(VERSION 3.14)

set(THIS ${PROJECT_NAME}Exe)

include(${AUGR_CMAKE}/Rack.cmake)
include(${AUGR_CMAKE}/RtAudio.cmake)

file(GLOB EXE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB EXE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${THIS} STATIC
  ${EXE_SRC}
  ${EXE_INCLUDE}
)
USES_RACK(${THIS})
USES_RTAUDIO(${THIS})
