#pragma once

namespace augr {

class Frame;

class Controller {
public:
    Controller(Frame &frame) : frame_(&frame) {}
    virtual ~Controller() = default;
    virtual void Control() = 0;

    Frame &frame() { return *frame_; }
    const Frame &frame() const { return *frame_; }

    Frame *frame_;
};

} // namespace augr