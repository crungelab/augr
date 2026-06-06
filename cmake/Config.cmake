include_guard()

#set(SHELL_PLATFORM Windows)
set(SHELL_PLATFORM X11)


set(AUGR_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(AUGR_CMAKE ${AUGR_ROOT}/cmake)
set(AUGR_DEPOT ${AUGR_ROOT}/depot)
set(AUGR_SRC ${AUGR_ROOT}/src)

set(AUGR_CORE_ROOT ${AUGR_SRC}/augr-core)
set(AUGR_RACK_ROOT ${AUGR_SRC}/augr-rack)
set(AUGR_EXE_ROOT ${AUGR_SRC}/augr-exe)
set(AUGR_FAUST_ROOT ${AUGR_SRC}/augr-faust)
set(AUGR_VOLT_ROOT ${AUGR_SRC}/augr-volt)
set(AUGR_FM_ROOT ${AUGR_SRC}/augr-fm)

set(STDUUID_ROOT ${AUGR_DEPOT}/stduuid)
set(FMT_ROOT ${AUGR_DEPOT}/fmt)
set(SPDLOG_ROOT ${AUGR_DEPOT}/spdlog)
set(SIGSLOT_ROOT ${AUGR_DEPOT}/sigslot)
set(JSON_ROOT ${AUGR_DEPOT}/json)

set(RTTR_ROOT ${AUGR_DEPOT}/rttr)
set(RTAUDIO_ROOT ${AUGR_DEPOT}/rtaudio)
set(RTMIDI_ROOT ${AUGR_DEPOT}/rtmidi)
set(MIDIFILE_ROOT ${AUGR_DEPOT}/midifile)
set(XTENSOR_ROOT ${AUGR_DEPOT}/xtensor)
set(XTL_ROOT ${AUGR_DEPOT}/xtl)
set(KISSFFT_ROOT ${AUGR_DEPOT}/kissfft)
set(PFD_ROOT ${AUGR_DEPOT}/portable-file-dialogs)

set(IMGUI_ROOT ${AUGR_DEPOT}/imgui)
set(IMGUI_EX ${AUGR_DEPOT}/imgui/backends)
set(IMPLOT_ROOT ${AUGR_DEPOT}/implot)
set(IMNODES_ROOT ${AUGR_DEPOT}/imnodes)
set(IMKNOBS_ROOT ${AUGR_DEPOT}/imknobs)

set(SDL_ROOT ${AUGR_DEPOT}/sdl)
