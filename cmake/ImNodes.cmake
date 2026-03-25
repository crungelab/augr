include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/ImGui.cmake)

function(USES_IMNODES THIS)
  USES_IMGUI(${THIS})
  target_include_directories(${THIS} PRIVATE
    ${IMNODES_ROOT}
  )
  target_link_libraries(${THIS} PRIVATE ImNodes)
endfunction()
