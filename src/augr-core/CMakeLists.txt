cmake_minimum_required(VERSION 3.14)

project(AugrCore LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

file(GLOB_RECURSE AUGR_SRC "src/*.cpp")
file(GLOB_RECURSE AUGR_INCLUDE CONFIGURE_DEPENDS "include/*.h")

add_library(${PROJECT_NAME} STATIC
  ${AUGR_SRC}
  ${AUGR_INCLUDE}
)
target_include_directories(${THIS} PRIVATE ${AUGR_CORE_ROOT}/include)

include(${AUGR_CMAKE}/XTensor.cmake)
USES_XTENSOR(${THIS})

add_subdirectory(src/midi)

