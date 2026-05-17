include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugiteShell.cmake)
#include(${CMAKE_CURRENT_LIST_DIR}/RtAudio.cmake)
#include(${CMAKE_CURRENT_LIST_DIR}/AugrRack.cmake)


function(USES_AUGITE_APP THIS)
USES_AUGITE_SHELL(${THIS})
#USES_RTAUDIO(${THIS})
#USES_AUGR_RACK(${THIS})
target_link_libraries(${THIS} PRIVATE AugiteApp)
endfunction()
