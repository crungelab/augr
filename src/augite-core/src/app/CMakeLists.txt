cmake_minimum_required(VERSION 3.14)

project(AugiteApp LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

include(${AUGR_CMAKE}/AugiteArchiver.cmake)
include(${AUGR_CMAKE}/AugiteShell.cmake)
include(${AUGR_CMAKE}/AugiteWidget.cmake)
include(${AUGR_CMAKE}/AugiteView.cmake)
include(${AUGR_CMAKE}/AugiteViewer.cmake)
include(${AUGR_CMAKE}/AugiteInspector.cmake)
include(${AUGR_CMAKE}/AugiteController.cmake)
include(${AUGR_CMAKE}/AugiteSystem.cmake)

file(GLOB APP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB APP_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC
  ${APP_SRC}
  ${APP_INCLUDE}
)

USES_AUGITE_ARCHIVER(${THIS})
USES_AUGITE_SHELL(${THIS})
USES_AUGITE_WIDGET(${THIS})
USES_AUGITE_VIEW(${THIS})
USES_AUGITE_VIEWER(${THIS})
USES_AUGITE_INSPECTOR(${THIS})
USES_AUGITE_CONTROLLER(${THIS})
USES_AUGITE_SYSTEM(${THIS})
