cmake_minimum_required(VERSION 3.14)

project(Augr LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

file(GLOB AUGR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB AUGR_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC
  ${AUGR_SRC}
  ${AUGR_INCLUDE}
)

include(${AUGR_CMAKE}/XTensor.cmake)
USES_XTENSOR(${THIS})

#add_subdirectory(vst3)
add_subdirectory(rack)

