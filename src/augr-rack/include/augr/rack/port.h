#pragma once

#include <vector>

#include <augr/rack/pin.h>

namespace augr {

class Port {
public:
    void AddPin(Pin &pin) { pins_.push_back(&pin); }

    Pin *FindPin(const std::string &name) const {
        for (Pin *p : pins_) {
            if (p->name() == name)
                return p;
        }
        return nullptr;
    }
    // Accessors
    int nPins() { return pins_.size(); }
    // Data members
    std::vector<Pin *> pins_;
};

class Inport : public Port {
public:
};

class Outport : public Port {
public:
};

} // namespace augr