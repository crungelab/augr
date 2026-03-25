cmake_minimum_required(VERSION 3.15)

project(daw LANGUAGES CXX)
set(THIS ${PROJECT_NAME})
set(THIS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB EXE_SOURCES ${THIS_DIR}/*.cpp)
file(GLOB EXE_INCLUDES ${THIS_DIR}/*.h)

add_executable(${THIS} "main.cpp" ${CMN_SOURCES} ${CMN_INCLUDES} ${EXE_SOURCES} ${EXE_INCLUDES})
target_include_directories(${THIS} PRIVATE ${PROJECT_ROOT})

include(${AUGR_CMAKE}/ExeApp.cmake)
include(${AUGR_CMAKE}/ExeRack.cmake)
include(${AUGR_CMAKE}/Library.cmake)
include(${THIS_DIR}/../../cmake/Common.cmake)

USES_EXE_APP(${THIS})
USES_EXE_RACK(${THIS})
USES_LIBRARY(${THIS})
USES_COMMON(${THIS})