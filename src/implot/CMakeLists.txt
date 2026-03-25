cmake_minimum_required(VERSION 3.15)

project(ImPlot LANGUAGES CXX)

set(IMPLOT_DIR ${AUGR_DEPOT}/implot)

set(IMPLOT_HEADERS
    ${IMPLOT_DIR}/implot_internal.h
    ${IMPLOT_DIR}/implot.h
)

set(IMPLOT_SRC
    ${IMPLOT_DIR}/implot_demo.cpp
    ${IMPLOT_DIR}/implot_items.cpp
    ${IMPLOT_DIR}/implot.cpp
)

add_library(ImPlot STATIC ${IMPLOT_SRC} ${IMPLOT_HEADERS})

include(${AUGR_CMAKE}/ImGui.cmake)
USES_IMGUI(ImPlot)