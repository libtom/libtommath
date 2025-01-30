#include "tommath_private.h"
#ifdef S_MP_RAND_SOURCE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err(*s_mp_rand_source)(void *out, size_t size) = s_mp_rand_platform;
#endif
