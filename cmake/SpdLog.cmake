include_guard()

function(USES_SPDLOG THIS)
  target_include_directories(${THIS} PRIVATE
    ${SPDLOG_ROOT/include}
  )
  target_link_libraries(${THIS} PRIVATE spdlog::spdlog)
endfunction()
