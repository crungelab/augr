cmake_minimum_required(VERSION 3.14)

set(THIS ${PROJECT_NAME}_vst3)

file(GLOB IMPL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB IMPL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${THIS} STATIC
  ${IMPL_SRC}
  ${IMPL_INCLUDE}
)

include(${AUGR_CMAKE}/Vst3.cmake)
USES_VST3(${THIS})

#target_include_directories(${THIS} ${CMAKE_CURRENT_SOURCE_DIR})
