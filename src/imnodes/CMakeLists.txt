cmake_minimum_required(VERSION 3.15)

project(ImNodes LANGUAGES CXX)

set(IMNODES_DIR ${AUGR_DEPOT}/imnodes)

set(IMNODES_HEADERS
    ${IMNODES_DIR}/imnodes_internal.h
    ${IMNODES_DIR}/imnodes.h
)

set(IMNODES_SRC
    ${IMNODES_DIR}/imnodes.cpp
)

add_library(ImNodes STATIC ${IMNODES_SRC} ${IMNODES_HEADERS})

include(${AUGR_CMAKE}/ImGui.cmake)
USES_IMGUI(ImNodes)