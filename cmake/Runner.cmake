include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrCore.cmake)

function(USES_RUNNER THIS)
  USES_AUGR_CORE(${THIS})
  target_include_directories(${THIS} PRIVATE
    ${RUNNER_INCLUDE_DIRS}
  )
  #target_link_libraries(${THIS} PRIVATE common)
endfunction()
