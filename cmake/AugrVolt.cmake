include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrRack.cmake)

function(INTERNAL_AUGR_VOLT THIS)
  USES_AUGR_RACK(${THIS})
  target_include_directories(${THIS} PRIVATE ${AUGR_VOLT_ROOT}/include)
endfunction()

function(USES_AUGR_VOLT THIS)
  INTERNAL_AUGR_VOLT(${THIS})
  target_link_libraries(${THIS} PRIVATE AugrVolt)
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()
