cmake_minimum_required(VERSION 3.14)

project(AugiteApp LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

include(${AUGR_CMAKE}/Shell.cmake)
include(${AUGR_CMAKE}/Rack.cmake)
include(${AUGR_CMAKE}/Widget.cmake)
include(${AUGR_CMAKE}/View.cmake)
include(${AUGR_CMAKE}/System.cmake)

file(GLOB APP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB APP_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC
  ${APP_SRC}
  ${APP_INCLUDE}
)

USES_SHELL(${THIS})
USES_RACK(${THIS})
USES_WIDGET(${THIS})
USES_VIEW(${THIS})
USES_SYSTEM(${THIS})
