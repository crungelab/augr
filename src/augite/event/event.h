#pragma once
#include <SDL3/SDL.h>
#include <cstdint>
#include <memory>

namespace augr
{

    inline Uint32 gUserEventType = 0;

    inline bool InitUserEvents()
    {
        gUserEventType = SDL_RegisterEvents(1);
        return gUserEventType != 0;
    }

    // ---- Base event ----
    struct Event
    {
        virtual ~Event() = default;
    };

    // ---- Post helper ----
    template <class T, class... Args>
    inline void Post(Args &&...args)
    {
        static_assert(std::is_base_of_v<Event, T>, "T must derive from sdl::Event");
        auto *payload = new T(std::forward<Args>(args)...);

        SDL_Event ev{};
        ev.type = gUserEventType;
        ev.user.code = T::code;
        ev.user.data1 = payload;
        ev.user.data2 = nullptr;

        SDL_PushEvent(&ev);
    }

    // ---- Decode helpers ----
    template <class T>
    inline T *TryDecode(const SDL_Event &e)
    {
        if (e.type == gUserEventType && e.user.code == T::code)
            return static_cast<T *>(e.user.data1);
        return nullptr;
    }

    template <class T>
    using Ptr = std::unique_ptr<T, void (*)(T *)>;

    template <class T>
    inline Ptr<T> Adopt(T *p)
    {
        return Ptr<T>(p, [](T *x)
                      { delete x; });
    }

// ---- Macro for convenience ----
#define DECLARE_SDL_EVENT(EVENT_NAME, CODE_VALUE)     \
    struct EVENT_NAME : public ::sdl::Event           \
    {                                                 \
        static constexpr int32_t code = (CODE_VALUE); \
        EVENT_NAME() = default;                       \
    };

} // namespace augr
