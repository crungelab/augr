include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugiteViewer.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/AugrFm.cmake)

function(INTERNAL_AUGITE_FM THIS)
  USES_AUGITE_VIEWER(${THIS})
  USES_AUGR_FM(${THIS})
  target_include_directories(${THIS} PRIVATE ${AUGITE_FM_ROOT}/include)
endfunction()

function(USES_AUGITE_FM THIS)
  INTERNAL_AUGITE_FM(${THIS})
  target_link_libraries(${THIS} PRIVATE AugiteFm)
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()
