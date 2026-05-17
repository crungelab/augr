include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugiteShell.cmake)


function(USES_AUGITE_APP THIS)
USES_AUGITE_SHELL(${THIS})
target_link_libraries(${THIS} PRIVATE AugiteApp)
endfunction()
