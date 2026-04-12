include_guard()

function(USES_MIDIFILE THIS)
  target_include_directories(${THIS} PRIVATE ${MIDIFILE_ROOT})  
  target_link_libraries(${THIS} PRIVATE midifile)
endfunction()