include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrCore.cmake)
include(${AUGR_CMAKE}/Pfd.cmake)

function(INTERNAL_AUGITE THIS)
  target_include_directories(Augite INTERFACE ${AUGR_ROOT}/src/augite-core/include)
endfunction()

function(USES_AUGITE THIS)
  INTERNAL_AUGITE(${THIS})
  USES_AUGR_CORE(${THIS})
  USES_PFD(${THIS})
  #target_include_directories(${THIS} PRIVATE ${AUGR_ROOT}/src)
  target_link_libraries(${THIS} PRIVATE Augite)
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()
