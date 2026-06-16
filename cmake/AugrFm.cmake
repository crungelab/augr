include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrRack.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/AugrVolt.cmake)

function(INTERNAL_AUGR_FM THIS)
  USES_AUGR_RACK(${THIS})
  USES_AUGR_VOLT(${THIS})
  target_include_directories(${THIS} PRIVATE ${AUGR_FM_ROOT}/include)
endfunction()

function(USES_AUGR_FM THIS)
  INTERNAL_AUGR_FM(${THIS})
  target_link_libraries(${THIS} PRIVATE AugrFm)
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()
