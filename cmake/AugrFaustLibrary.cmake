include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrFaust.cmake)

function(USES_AUGR_FAUST_LIBRARY THIS)
USES_AUGR_FAUST(${THIS})
target_link_libraries(${THIS} PRIVATE AugrFaustLibrary)
endfunction()
