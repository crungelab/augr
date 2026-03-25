include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrCore.cmake)

function(USES_RACK THIS)
USES_AUGR_CORE(${THIS})
target_link_libraries(${THIS} PRIVATE AugrRack)
endfunction()
