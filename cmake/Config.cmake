include_guard()

#set(SHELL_PLATFORM Windows)
set(SHELL_PLATFORM X11)


set(AUGR_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(AUGR_CMAKE ${AUGR_ROOT}/cmake)
set(AUGR_DEPOT ${AUGR_ROOT}/depot)

set(FMT_ROOT ${AUGR_DEPOT}/fmt)
set(RTTR_ROOT ${AUGR_DEPOT}/rttr)
set(RTAUDIO_ROOT ${AUGR_DEPOT}/rtaudio)
set(XTENSOR_ROOT ${AUGR_DEPOT}/xtensor)
set(XTL_ROOT ${AUGR_DEPOT}/xtl)
set(IMGUI_ROOT ${AUGR_DEPOT}/imgui)
set(IMGUI_EX ${AUGR_DEPOT}/imgui/backends)
set(IMPLOT_ROOT ${AUGR_DEPOT}/implot)
set(IMNODES_ROOT ${AUGR_DEPOT}/imnodes)
set(IMKNOBS_ROOT ${AUGR_DEPOT}/imknobs)

set(SDL_ROOT ${AUGR_DEPOT}/sdl)
