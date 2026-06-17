cmake_minimum_required(VERSION 3.14)

project(Augite LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

file(GLOB AUGITE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB AUGITE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_library(${PROJECT_NAME} STATIC
  ${AUGITE_SRC}
  ${AUGITE_INCLUDE}
)

target_include_directories(${THIS} PUBLIC ${AUGR_ROOT}/src/augite-core/include)

add_subdirectory(src/archiver)
add_subdirectory(src/shell)
add_subdirectory(src/vst3)
add_subdirectory(src/widget)
add_subdirectory(src/view)
add_subdirectory(src/viewer)
add_subdirectory(src/inspector)
add_subdirectory(src/controller)
add_subdirectory(src/app)
add_subdirectory(src/system)

