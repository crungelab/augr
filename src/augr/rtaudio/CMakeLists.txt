cmake_minimum_required(VERSION 3.14)

project(AugrRtAudio LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

file(GLOB AUGR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB AUGR_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(${PROJECT_NAME} STATIC
  ${AUGR_SRC}
  ${AUGR_INCLUDE}
)
target_include_directories(${THIS} PRIVATE ${AUGR_RTAUDIO_ROOT}/include)

include(${AUGR_CMAKE}/AugrCore.cmake)
INTERNAL_AUGR_CORE(${THIS})

add_subdirectory(src/rack)

