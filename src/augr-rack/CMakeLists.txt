cmake_minimum_required(VERSION 3.14)

project(AugrRack LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

file(GLOB AUGR_SRC "src/*.cpp")
file(GLOB_RECURSE AUGR_INCLUDE CONFIGURE_DEPENDS "include/*.h")

add_library(${PROJECT_NAME} STATIC
  ${AUGR_SRC}
  ${AUGR_INCLUDE}
)
target_include_directories(${THIS} PRIVATE ${AUGR_RACK_ROOT}/include)

include(${AUGR_CMAKE}/AugrRack.cmake)
INTERNAL_AUGR_RACK(${THIS})

add_subdirectory(src)

