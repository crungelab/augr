cmake_minimum_required(VERSION 3.14)

project(AugiteFm LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

file(GLOB_RECURSE AUGITE_FM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE AUGITE_FM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(${PROJECT_NAME} STATIC
  ${AUGITE_FM_SRC}
  ${AUGITE_FM_INCLUDE}
)
target_include_directories(${THIS} PRIVATE ${AUGITE_FM_ROOT}/include)

include(${AUGR_CMAKE}/AugiteFm.cmake)
INTERNAL_AUGITE_FM(${THIS})

include(${AUGR_CMAKE}/AugrRack.cmake)
USES_AUGR_RACK(${THIS})

include(${AUGR_CMAKE}/AugrFm.cmake)
USES_AUGR_FM(${THIS})
