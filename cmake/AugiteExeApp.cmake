include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugiteApp.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/RtAudio.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/RtMidi.cmake)

function(USES_AUGITE_EXE_APP THIS)
USES_AUGITE_APP(${THIS})
USES_RTAUDIO(${THIS})
USES_RTMIDI(${THIS})
endfunction()
