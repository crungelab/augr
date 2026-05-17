#pragma once

namespace augr {

class Pin;
class Connection;

class Wire {
public:
    Wire(Pin &output, Pin &input);
    ~Wire();
    // Data members
    Pin *output_ = nullptr;
    Pin *input_ = nullptr;
    Connection *connection_ = nullptr;
    //
    static int next_id_;
    int id_ = 0;
};

} // namespace augr