cmake_minimum_required(VERSION 3.15)

project(ImGui LANGUAGES CXX)

set(_ ${IMGUI_ROOT})

set(IMGUI_HEADERS
    ${_}/imconfig.h
    ${_}/imgui_internal.h
    ${_}/imgui.h
    ${_}/imstb_rectpack.h
    ${_}/imstb_textedit.h
    ${_}/imstb_truetype.h
)

set(IMGUI_SRC
    ${_}/imgui_demo.cpp
    ${_}/imgui_draw.cpp
    ${_}/imgui_widgets.cpp
    ${_}/imgui_tables.cpp
    ${_}/imgui.cpp
)

add_library(ImGui STATIC ${IMGUI_SRC} ${IMGUI_HEADERS})