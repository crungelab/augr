include_guard()

function(USES_XTL THIS)
target_include_directories(${THIS} PRIVATE ${XTL_ROOT}/include)
target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()

function(USES_XTENSOR THIS)
USES_XTL(${THIS})
target_include_directories(${THIS} PRIVATE ${XTENSOR_ROOT}/include)  
endfunction()
