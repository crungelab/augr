#pragma once

#ifdef SHELL_HEADER
#include SHELL_HEADER
#else
#include "base_window.h"
#endif

namespace augr {

#ifdef SHELL_CLASS
using Window = SHELL_CLASS;
#else
using Window = BaseWindow;
#endif

} // namespace augr