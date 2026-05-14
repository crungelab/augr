cmake_minimum_required(VERSION 3.14)

set(THIS AugiteFrame)

project(${THIS} CXX)

include(${AUGR_CMAKE}/AugrRack.cmake)
include(${AUGR_CMAKE}/Augite.cmake)
include(${AUGR_CMAKE}/View.cmake)

include(${AUGR_CMAKE}/ImPlot.cmake)
include(${AUGR_CMAKE}/ImNodes.cmake)
include(${AUGR_CMAKE}/ImKnobs.cmake)

file(GLOB FRAME_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB FRAME_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC
  ${FRAME_SRC}
  ${FRAME_INCLUDE}
)

USES_AUGR_RACK(${THIS})
USES_AUGITE(${THIS})
USES_VIEW(${THIS})

USES_IMPLOT(${THIS})
USES_IMNODES(${THIS})
USES_IMKNOBS(${THIS})
