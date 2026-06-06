include_guard()

function(USES_SIGSLOT THIS)
  target_link_libraries(${THIS} PRIVATE Pal::Sigslot)
endfunction()
