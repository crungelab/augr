cmake_minimum_required(VERSION 3.15)

project(multi LANGUAGES CXX)
set(THIS ${PROJECT_NAME})
set(THIS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB EXE_SOURCES ${THIS_DIR}/*.cpp)
file(GLOB EXE_INCLUDES ${THIS_DIR}/*.h)

add_executable(${THIS} "main.cpp" ${CMN_SOURCES} ${CMN_INCLUDES} ${EXE_SOURCES} ${EXE_INCLUDES})
target_include_directories(${THIS} PRIVATE ${PROJECT_ROOT})

include(${AUGR_CMAKE}/ExeApp.cmake)
include(${THIS_DIR}/../../cmake/Common.cmake)
include(${AUGR_CMAKE}/AugrRtAudio.cmake)

USES_EXE_APP(${THIS})
USES_AUGR_RTAUDIO(${THIS})
USES_COMMON(${THIS})