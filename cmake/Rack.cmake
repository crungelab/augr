include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Augr.cmake)

function(USES_RACK THIS)
USES_AUGR(${THIS})
target_link_libraries(${THIS} PRIVATE AugrRack)
endfunction()
