cmake_minimum_required(VERSION 3.15)

include(${AUGR_CMAKE}/ImGui.cmake)

set(THIS ImKnobs)

project(${THIS} LANGUAGES CXX)

set(IMKNOBS_HEADERS
  ${IMKNOBS_ROOT}/imgui-knobs.h
)

set(IMKNOBS_SRC
  ${IMKNOBS_ROOT}/imgui-knobs.cpp
)

add_library(${THIS} STATIC ${IMKNOBS_SRC} ${IMKNOBS_HEADERS})
USES_IMGUI(${THIS})