#pragma once

#include <vector>
#include <augr/core/rack/pin.h>


class Connector {
public:
    std::vector<Pin*> pins;
};