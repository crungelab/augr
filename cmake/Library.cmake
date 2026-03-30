include_guard()

function(USES_LIBRARY THIS)
target_link_libraries(${THIS} PRIVATE AugrFaustLibrary)
endfunction()
