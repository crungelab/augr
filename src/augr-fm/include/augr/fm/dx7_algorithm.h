// augr/fm/dx7_algorithm.h
#pragma once

#include <array>
#include <cstdint>

namespace augr::fm {

struct Dx7ModRoute {
    int src;
    int dst;
};

struct Dx7AlgorithmDef {
    std::array<bool, 6>          is_carrier;
    std::array<Dx7ModRoute, 5>   routes;
    int                          route_count;
    int                          feedback_op;     // operator index that receives patch feedback, or -1 if none
    float                        feedback_scale;  // per-algorithm feedback depth constant (e.g. 0.1, 0.2, 0.4)
};

const Dx7AlgorithmDef& GetDx7Algorithm(int index);

} // namespace augr::fm