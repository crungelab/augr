include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Fmt.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/XTensor.cmake)

function(USES_AUGR THIS)
  USES_FMT(${THIS})
  USES_XTENSOR(${THIS})
  target_include_directories(${THIS} PRIVATE ${AUGR_ROOT}/include)
  target_link_libraries(${THIS} PRIVATE Augr)
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()
