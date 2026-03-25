include_guard()

function(USES_RTAUDIO THIS)
  target_include_directories(${THIS} PRIVATE ${RTAUDIO_ROOT})  
  target_link_libraries(${THIS} PRIVATE rtaudio)
endfunction()
