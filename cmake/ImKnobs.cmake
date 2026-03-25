include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/ImGui.cmake)

function(USES_IMKNOBS THIS)
  USES_IMGUI(${THIS})
  target_include_directories(${THIS} PRIVATE
    ${IMKNOBS_ROOT}
  )
  target_link_libraries(${THIS} PRIVATE ImKnobs)
endfunction()
