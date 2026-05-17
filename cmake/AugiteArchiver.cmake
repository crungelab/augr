include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrRack.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Augite.cmake)

function(USES_AUGITE_ARCHIVER THIS)
  USES_AUGR_RACK(${THIS})
  USES_AUGITE(${THIS})
  target_link_libraries(${THIS} PRIVATE AugiteArchiver)
endfunction()
