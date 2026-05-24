cmake_minimum_required(VERSION 3.15)

include(cmake/Config.cmake)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
#set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(AugrAll)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#STDUUID BEGIN
add_subdirectory(${STDUUID_ROOT})
#STDUUID END

#FMT BEGIN
add_subdirectory(${FMT_ROOT})
#FMT END

#SPDLOG BEGIN
add_subdirectory(${SPDLOG_ROOT})
#SPDLOG END

#JSON BEGIN
add_subdirectory(${JSON_ROOT})
#JSON END

#RTAUDIO BEGIN
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(${RTAUDIO_ROOT})
#RTAUDIO END

#RTMIDI BEGIN
set(RTMIDI_API_ALSA ON CACHE BOOL "Compile RtMidi with ALSA support" FORCE)
add_subdirectory(${RTMIDI_ROOT})
#RTMIDI END

#MIDIFILE BEGIN
#add_subdirectory(${MIDIFILE_ROOT})
add_subdirectory(${AUGR_ROOT}/src/midifile)
#MIDIFILE END

#XTL BEGIN
add_subdirectory(${XTL_ROOT})
#XTL END

#XTENSOR BEGIN
add_subdirectory(${XTENSOR_ROOT})
#XTENSOR END

#KISSFFT BEGIN
add_subdirectory(${KISSFFT_ROOT})
#KISSFFT END

#PFD BEGIN
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
add_subdirectory(${PFD_ROOT})
#PFD END

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


#AugrCore
add_subdirectory(src/augr-core)

#AugrRack
add_subdirectory(src/augr-rack)

#AugrFaust
add_subdirectory(src/augr-faust)

#AugrFaustLibrary
add_subdirectory(src/augr-faust-library)

#AugrVolt
add_subdirectory(src/augr-volt)

#AugrFm
add_subdirectory(src/augr-fm)

#AugrExe
add_subdirectory(src/augr-exe)

#Augite
add_subdirectory(src/augite)

#Tests
add_subdirectory(tests)

#Examples
add_subdirectory(examples)
