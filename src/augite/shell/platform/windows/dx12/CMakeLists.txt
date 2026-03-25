cmake_minimum_required(VERSION 3.14)

set(THIS ${PROJECT_NAME}${CMAKE_SYSTEM_NAME}Dx12)

include(${AUGR_CMAKE}/Augr.cmake)
include(${AUGR_CMAKE}/ImGui.cmake)

file(GLOB SHELL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SHELL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${THIS} STATIC
  #${CMAKE_CURRENT_SOURCE_DIR}/../../window.cpp
  ${SHELL_SRC}
  ${SHELL_INCLUDE}

  ${IMGUI_EX}/imgui_impl_dx12.cpp
)
USES_AUGR(${THIS})
USES_IMGUI(${THIS})

target_link_libraries(${THIS} PRIVATE ${PROJECT_NAME}${CMAKE_SYSTEM_NAME} AugiteShell D3d12 dxgi)
