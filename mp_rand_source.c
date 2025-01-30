#include "tommath_private.h"
#ifdef MP_RAND_SOURCE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
void mp_rand_source(mp_err(*source)(void *out, size_t size))
{
   if (source == NULL)
      s_mp_rand_source = s_mp_rand_platform;
   else
      s_mp_rand_source = source;
}
#endif
