cmake_minimum_required(VERSION 3.15)

project(panner LANGUAGES CXX VERSION 1.0.0)
set(BINARY_NAME "panner")

set(THIS ${BINARY_NAME})

file(GLOB RUNNER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB RUNNER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

include(${AUGR_CMAKE}/App.cmake)

add_executable(${THIS} "main.cpp" ${RUNNER_SOURCES} ${RUNNER_INCLUDES})
USES_EXE_APP(${THIS})

add_library(${THIS}_lib ${RUNNER_SOURCES})
USES_EXE_APP(${THIS}_lib)

target_compile_definitions(${THIS} PRIVATE "NOMINMAX" RACK_IMPL)

add_subdirectory(vst3)

add_subdirectory(format)
