#pragma once

#include "event.h"

namespace augr
{
    struct ResizeEvent : public Event
    {
        static constexpr int32_t code = 1;
        ResizeEvent(int w, int h) : width(w), height(h) {}
        int width{};
        int height{};
    };

    struct CloseEvent : public Event
    {
        static constexpr int32_t code = 2;
    };

} // namespace augr