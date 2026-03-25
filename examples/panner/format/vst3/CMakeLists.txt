cmake_minimum_required(VERSION 3.15)

set(THIS ${PROJECT_NAME}_x11_vst3)

file(GLOB VST3_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB VST3_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

#add_library(${THIS} SHARED ${VST3_SOURCES} ${VST3_INCLUDE})
smtg_add_vst3plugin(${THIS} ${VST3_SOURCES} ${VST3_INCLUDE})

include(${AUGR_CMAKE}/Vst3.cmake)
USES_VST3(${THIS})

include(${AUGR_CMAKE}/ImGui.cmake)
USES_IMGUI(${THIS})

target_compile_definitions(${THIS} PRIVATE "NOMINMAX" RACK_IMPL)

#target_link_libraries(${THIS} PRIVATE ${RUNNER_LIBS})
target_link_libraries(${THIS} PRIVATE ${PROJECT_NAME}_lib ${PROJECT_NAME}_vst3)
target_include_directories(${THIS} PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../
  ${CMAKE_CURRENT_SOURCE_DIR}/../../vst3
)
#add_dependencies(${THIS} ${RUNNER_DEPS})

if(MSVC)
  set_target_properties(${THIS}
      PROPERTIES 
          LINK_FLAGS          /EXPORT:GetPluginFactory
  )
endif(MSVC)

if(${SHELL_PLATFORM} STREQUAL "Windows")
set_target_properties(${THIS} PROPERTIES SUFFIX ".vst3")
else()
#set_target_properties(${THIS} PROPERTIES SUFFIX ".vst.so")
endif()

#add_custom_command(
#    TARGET ${THIS}
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy
#        $<TARGET_FILE:${THIS}>
#        ${AUGR_ROOT}/vst3
#)
