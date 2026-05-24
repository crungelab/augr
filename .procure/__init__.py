from procure import GitSolution


class StdUuid(GitSolution):
    path = "depot/std-uuid"
    url = "https://github.com/mariusbancila/stduuid"


class Fmt(GitSolution):
    path = "depot/fmt"
    url = "https://github.com/fmtlib/fmt"


class SpdLog(GitSolution):
    path = "depot/spdlog"
    url = "https://github.com/gabime/spdlog"


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


class MidiFile(GitSolution):
    path = "depot/midifile"
    url = "https://github.com/craigsapp/midifile"


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


class KissFFT(GitSolution):
    path = "depot/kissfft"
    url = "https://github.com/mborgerding/kissfft"


class Pfd(GitSolution):
    path = "depot/portable-file-dialogs"
    url = "https://github.com/samhocevar/portable-file-dialogs"


class Faust(GitSolution):
    path = "depot/faust"
    url = "https://github.com/grame-cncm/faust"
    recursive = True


solutions = [
    StdUuid,
    Fmt,
    SpdLog,
    SDL,
    ImGui,
    ImGuiKnobs,
    ImNodes,
    ImPlot,
    RtAudio,
    RtMidi,
    MidiFile,
    VST3SDK,
    Xtensor,
    Xtl,
    KissFFT,
    Pfd,
    Faust,
]
