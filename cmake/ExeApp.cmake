include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/App.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/RtAudio.cmake)

function(USES_EXE_APP THIS)
USES_APP(${THIS})
USES_RTAUDIO(${THIS})
#target_link_libraries(${THIS} AugiteApp)
endfunction()
