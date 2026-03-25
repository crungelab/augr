include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Shell.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/RtAudio.cmake)

function(USES_APP THIS)
USES_SHELL(${THIS})
USES_RTAUDIO(${THIS})
target_link_libraries(${THIS} PRIVATE AugiteApp)
endfunction()
