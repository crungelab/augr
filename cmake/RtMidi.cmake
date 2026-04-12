include_guard()

function(USES_RTMIDI THIS)
  target_include_directories(${THIS} PRIVATE ${RTMIDI_ROOT})  
  target_link_libraries(${THIS} PRIVATE rtmidi)
endfunction()
