from procure import GitSolution


class Faust(GitSolution):
    path = "depot/faust"
    url = "https://github.com/grame-cncm/faust"
    recursive = True


class Fmt(GitSolution):
    path = "depot/fmt"
    url = "https://github.com/fmtlib/fmt"


class SDL(GitSolution):
    path = "depot/sdl"
    url = "https://github.com/libsdl-org/SDL"


class ImGui(GitSolution):
    path = "depot/imgui"
    url = "https://github.com/ocornut/imgui"
    branch = "docking"


class ImGuiKnobs(GitSolution):
    path = "depot/imknobs"
    url = "https://github.com/altschuler/imgui-knobs"


class ImNodes(GitSolution):
    path = "depot/imnodes"
    url = "https://github.com/Nelarius/imnodes"


class ImPlot(GitSolution):
    path = "depot/implot"
    url = "https://github.com/epezent/implot"


class RtAudio(GitSolution):
    path = "depot/rtaudio"
    url = "https://github.com/thestk/rtaudio"


class RtMidi(GitSolution):
    path = "depot/rtmidi"
    url = "https://github.com/thestk/rtmidi"


class VST3SDK(GitSolution):
    path = "depot/vst3sdk"
    url = "https://github.com/steinbergmedia/vst3sdk"
    recursive = True


class Xtensor(GitSolution):
    path = "depot/xtensor"
    url = "https://github.com/xtensor-stack/xtensor"


class Xtl(GitSolution):
    path = "depot/xtl"
    url = "https://github.com/xtensor-stack/xtl"


solutions = [
    Faust,
    Fmt,
    SDL,
    ImGui,
    ImGuiKnobs,
    ImNodes,
    ImPlot,
    RtAudio,
    RtMidi,
    VST3SDK,
    Xtensor,
    Xtl,
]
