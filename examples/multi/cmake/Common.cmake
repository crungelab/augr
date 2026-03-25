include_guard()

include(${AUGR_CMAKE}/AugrCore.cmake)

function(USES_COMMON THIS)
USES_AUGR_CORE(${THIS})
target_include_directories(${THIS} PRIVATE ${PROJECT_ROOT})
endfunction()
