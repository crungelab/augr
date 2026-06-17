cmake_minimum_required(VERSION 3.14)

set(THIS AugiteWidget)

project(${THIS} CXX)

include(${AUGR_CMAKE}/AugrRack.cmake)
include(${AUGR_CMAKE}/Augite.cmake)
include(${AUGR_CMAKE}/ImPlot.cmake)
include(${AUGR_CMAKE}/ImNodes.cmake)
include(${AUGR_CMAKE}/ImKnobs.cmake)

file(GLOB WIDGET_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB WIDGET_INCLUDE ${AUGR_ROOT}/src/augite-core/include/widget/*.h)

add_library(${PROJECT_NAME} STATIC
  ${WIDGET_SRC}
  ${WIDGET_INCLUDE}
)

USES_AUGR_RACK(${THIS})
USES_AUGITE(${THIS})
USES_IMPLOT(${THIS})
USES_IMNODES(${THIS})
USES_IMKNOBS(${THIS})
