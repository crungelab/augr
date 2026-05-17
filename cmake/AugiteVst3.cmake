include_guard()
cmake_minimum_required(VERSION 3.15)

include(${CMAKE_CURRENT_LIST_DIR}/AugiteShell.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/VST3SDK.cmake)

function(USES_AUGITE_VST3 THIS)
  USES_AUGITE_SHELL(${THIS})
  USES_AUGITE_VST3SDK(${THIS})
  USES_AUGITE(${THIS})
  target_link_libraries(${THIS} PRIVATE AugiteVst3 AugiteShell${SHELL_PLATFORM})
endfunction()
