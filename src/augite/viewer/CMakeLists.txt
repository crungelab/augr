cmake_minimum_required(VERSION 3.14)

set(THIS AugiteViewer)

project(${THIS} CXX)

include(${AUGR_CMAKE}/AugrRack.cmake)
include(${AUGR_CMAKE}/AugrFm.cmake)

include(${AUGR_CMAKE}/Augite.cmake)
include(${AUGR_CMAKE}/AugiteView.cmake)
include(${AUGR_CMAKE}/AugiteController.cmake)

include(${AUGR_CMAKE}/ImPlot.cmake)
include(${AUGR_CMAKE}/ImNodes.cmake)
include(${AUGR_CMAKE}/ImKnobs.cmake)

file(GLOB VIEWER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB VIEWER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC
  ${VIEWER_SRC}
  ${VIEWER_INCLUDE}
)

USES_AUGR_RACK(${THIS})
USES_AUGR_FM(${THIS})
USES_AUGITE(${THIS})
USES_AUGITE_VIEW(${THIS})
USES_AUGITE_CONTROLLER(${THIS})

USES_IMPLOT(${THIS})
USES_IMNODES(${THIS})
USES_IMKNOBS(${THIS})
