cmake_minimum_required(VERSION 3.14)

set(THIS AugiteView)

project(${THIS} CXX)

include(${AUGR_CMAKE}/AugrCore.cmake)
include(${AUGR_CMAKE}/Augite.cmake)
include(${AUGR_CMAKE}/Rack.cmake)
include(${AUGR_CMAKE}/Widget.cmake)

include(${AUGR_CMAKE}/ImPlot.cmake)
include(${AUGR_CMAKE}/ImNodes.cmake)
include(${AUGR_CMAKE}/ImKnobs.cmake)

file(GLOB VIEW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB VIEW_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC
  ${VIEW_SRC}
  ${VIEW_INCLUDE}
)

USES_AUGR_CORE(${THIS})
USES_AUGITE(${THIS})
USES_RACK(${THIS})
USES_WIDGET(${THIS})

USES_IMPLOT(${THIS})
USES_IMNODES(${THIS})
USES_IMKNOBS(${THIS})
