include_guard()

function(USES_JSON THIS)
  target_include_directories(${THIS} PRIVATE
    ${JSON_ROOT/include}
  )
  target_link_libraries(${THIS} PRIVATE nlohmann_json)
endfunction()
