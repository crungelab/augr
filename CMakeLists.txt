cmake_minimum_required(VERSION 3.15)

include(cmake/Config.cmake)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
#set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(AugrAll)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#FMT BEGIN
add_subdirectory(${FMT_ROOT})
#FMT END

#RTAUDIO BEGIN
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(${RTAUDIO_ROOT})
#RTAUDIO END

#XTL BEGIN
add_subdirectory(${XTL_ROOT})
#XTL END

#XTENSOR BEGIN
add_subdirectory(${XTENSOR_ROOT})
#XTENSOR END

#IMGUI BEGIN
add_subdirectory(${AUGR_ROOT}/src/imgui)
#IMGUI END

#IMPLOT BEGIN
add_subdirectory(${AUGR_ROOT}/src/implot)
#IMPLOT END

#IMNODES BEGIN
add_subdirectory(${AUGR_ROOT}/src/imnodes)
#IMNODES END

#IMKNOBS BEGIN
add_subdirectory(${AUGR_ROOT}/src/imknobs)
#IMKNOBS END

#VST3SDK BEGIN
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "Enable VST 3 Plug-in Examples" FORCE)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "Enable VST 3 Hosting Examples" FORCE)
set(SMTG_ENABLE_VSTGUI_SUPPORT OFF CACHE BOOL "Enable VSTGUI Support" FORCE)
add_subdirectory(${AUGR_DEPOT}/vst3sdk)
#VST3SDK END

#
# SDL
#
set(HAVE_LIBC ON)
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
set(SDL_TEST OFF CACHE BOOL "Build the SDL3_test library" FORCE)

set(SDL_RENDER OFF CACHE BOOL "Use render subsystem" FORCE)

add_subdirectory(depot/sdl)


#Augr
add_subdirectory(src/augr/core)

#Augite
add_subdirectory(src/augite)

#Examples
add_subdirectory(examples)

#Library
add_subdirectory(library)
