cmake_minimum_required(VERSION 3.14)

project(AugrFaust LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

file(GLOB AUGR_FAUST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB AUGR_FAUST_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(${PROJECT_NAME} STATIC
  ${AUGR_FAUST_SRC}
  ${AUGR_FAUST_INCLUDE}
)
target_include_directories(${THIS} PRIVATE ${AUGR_FAUST_ROOT}/include)

include(${AUGR_CMAKE}/AugrRack.cmake)
USES_AUGR_RACK(${THIS})
