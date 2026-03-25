include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Rack.cmake)

function(USES_EXE_RACK THIS)
USES_RACK(${THIS})
target_link_libraries(${THIS} PRIVATE AugrRackExe)
endfunction()
