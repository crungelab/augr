include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrRack.cmake)

function(INTERNAL_AUGR_FAUST THIS)
  USES_AUGR_RACK(${THIS})
  target_include_directories(${THIS} PRIVATE ${AUGR_FAUST_ROOT}/include)
endfunction()

function(USES_AUGR_FAUST THIS)
  INTERNAL_AUGR_FAUST(${THIS})
  target_link_libraries(${THIS} PRIVATE AugrFaust)
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()
