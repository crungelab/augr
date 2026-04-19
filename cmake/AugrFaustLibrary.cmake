include_guard()

function(USES_AUGR_FAUST_LIBRARY THIS)
target_link_libraries(${THIS} PRIVATE AugrFaustLibrary)
endfunction()
