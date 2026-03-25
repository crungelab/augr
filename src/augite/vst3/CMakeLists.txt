cmake_minimum_required(VERSION 3.14)

project(AugiteVst3 LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

include(${AUGR_CMAKE}/Augr.cmake)
include(${AUGR_CMAKE}/Augite.cmake)
include(${AUGR_CMAKE}/VST3SDK.cmake)

file(GLOB SHELL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SHELL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${THIS} STATIC
  ${SHELL_SRC}
  ${SHELL_INCLUDE}
)
USES_AUGR(${THIS})
USES_AUGITE(${THIS})
USES_VST3SDK(${THIS})

#target_link_libraries(${THIS} D3d12 dxgi)
