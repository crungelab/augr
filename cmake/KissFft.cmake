include_guard()

function(USES_KISSFFT THIS)
  target_include_directories(${THIS} PRIVATE
    ${KISSFFT_ROOT/include}
  )
  target_link_libraries(${THIS} PRIVATE kissfft)
endfunction()
