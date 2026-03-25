include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Fmt.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/XTensor.cmake)

function(INTERNAL_AUGR_CORE THIS)
  USES_FMT(${THIS})
  USES_XTENSOR(${THIS})
  target_include_directories(${THIS} PRIVATE ${AUGR_CORE_ROOT}/include)
endfunction()

function(USES_AUGR_CORE THIS)
  INTERNAL_AUGR_CORE(${THIS})
  target_link_libraries(${THIS} PRIVATE AugrCore)
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()
