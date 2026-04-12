include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrCore.cmake)

function(INTERNAL_AUGR_RACK THIS)
  target_include_directories(${THIS} PRIVATE ${AUGR_RACK_ROOT}/include)
endfunction()

function(USES_AUGR_RACK THIS)
  USES_AUGR_CORE(${THIS})
  INTERNAL_AUGR_RACK(${THIS})
  target_link_libraries(${THIS} PRIVATE AugrRack)
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()
