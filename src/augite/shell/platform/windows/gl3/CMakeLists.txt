cmake_minimum_required(VERSION 3.14)

set(THIS ${PROJECT_NAME}${CMAKE_SYSTEM_NAME}Gl3)

include(${AUGR_CMAKE}/AugrCore.cmake)
inclue(${AUGR_CMAKE}/Glfw.cmake)
include(${AUGR_CMAKE}/ImGui.cmake)

file(GLOB SHELL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SHELL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${THIS} STATIC
  ${SHELL_SRC}
  ${SHELL_INCLUDE}
  ${IMGUI_EX}/imgui_impl_opengl3.cpp
)
USES_AUGR_CORE(${THIS})
USES_SDL(${THIS})
USES_IMGUI(${THIS})

target_link_libraries(${THIS} PRIVATE ${PROJECT_NAME}${CMAKE_SYSTEM_NAME} AugiteShell)
