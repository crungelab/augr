cmake_minimum_required(VERSION 3.14)

project(AugiteShell LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

include(${AUGR_CMAKE}/Augr.cmake)
include(${AUGR_CMAKE}/Augite.cmake)
include(${AUGR_CMAKE}/ImGui.cmake)
include(${AUGR_CMAKE}/Sdl.cmake)

file(GLOB SHELL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SHELL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${THIS} STATIC
  ${SHELL_SRC}
  ${SHELL_INCLUDE}
  ${IMGUI_EX}/imgui_impl_sdl3.cpp
)

USES_AUGR(${THIS})
USES_AUGITE(${THIS})
USES_IMGUI(${THIS})
USES_SDL(${THIS})

if(${SHELL_PLATFORM} STREQUAL "X11")
  add_subdirectory(platform/x11)
elseif(${SHELL_PLATFORM} STREQUAL "Windows")
  add_subdirectory(platform/windows)
endif()