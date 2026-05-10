include_guard()

function(USES_PFD THIS)
  target_include_directories(${THIS} PRIVATE
    ${PFD_ROOT}
  )
  target_link_libraries(${THIS} PRIVATE portable_file_dialogs)
endfunction()
