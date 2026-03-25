#pragma once

#if defined AUGR_QUAD
typedef long double fy_real;
#elif defined AUGR_DOUBLE
typedef double fy_real;
#else
typedef float fy_real;
#endif

typedef fy_real* fy_buffer_t;
typedef fy_real** fy_buffers_t;

#define FAUSTFLOAT fy_real
