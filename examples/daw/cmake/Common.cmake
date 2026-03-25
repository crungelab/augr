include_guard()

include(${AUGR_CMAKE}/Augr.cmake)

function(USES_COMMON THIS)
USES_AUGR(${THIS})
target_include_directories(${THIS} PRIVATE ${PROJECT_ROOT})
endfunction()
