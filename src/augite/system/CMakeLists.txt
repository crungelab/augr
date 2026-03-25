cmake_minimum_required(VERSION 3.14)

set(THIS AugiteSystem)

project(${THIS} CXX)

include(${AUGR_CMAKE}/AugrCore.cmake)
include(${AUGR_CMAKE}/Augite.cmake)
include(${AUGR_CMAKE}/ImPlot.cmake)
include(${AUGR_CMAKE}/ImNodes.cmake)
include(${AUGR_CMAKE}/ImKnobs.cmake)

file(GLOB CONTEXT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB CONTEXT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC
  ${CONTEXT_SRC}
  ${CONTEXT_INCLUDE}
)

USES_AUGR_CORE(${THIS})
USES_AUGITE(${THIS})
USES_IMPLOT(${THIS})
USES_IMNODES(${THIS})
USES_IMKNOBS(${THIS})
