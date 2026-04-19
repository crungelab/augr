cmake_minimum_required(VERSION 3.14)

project(AugrVolt LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

file(GLOB AUGR_VOLT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB AUGR_VOLT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(${PROJECT_NAME} STATIC
  ${AUGR_VOLT_SRC}
  ${AUGR_VOLT_INCLUDE}
)
target_include_directories(${THIS} PRIVATE ${AUGR_VOLT_ROOT}/include)

include(${AUGR_CMAKE}/AugrRack.cmake)
USES_AUGR_RACK(${THIS})
