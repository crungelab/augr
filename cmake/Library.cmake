include_guard()

function(USES_LIBRARY THIS)
target_link_libraries(${THIS} PRIVATE fausty_dsp)
#target_include_directories(${THIS} PUBLIC "${FAUST_GEN_DIR}")
endfunction()
