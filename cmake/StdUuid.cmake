include_guard()

function(USES_STDUUID THIS)
  target_include_directories(${THIS} PRIVATE
    ${STDUUID_ROOT/include}
  )
  target_link_libraries(${THIS} PRIVATE stduuid)
endfunction()
