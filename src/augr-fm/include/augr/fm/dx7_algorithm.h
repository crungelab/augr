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
    int                          feedback_op;  // operator index that receives patch feedback
};

const Dx7AlgorithmDef& GetDx7Algorithm(int index);

} // namespace augr::fm