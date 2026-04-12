include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/App.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/RtAudio.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/RtMidi.cmake)

function(USES_EXE_APP THIS)
USES_APP(${THIS})
USES_RTAUDIO(${THIS})
USES_RTMIDI(${THIS})
endfunction()
