#include "tommath_private.h"
/* MP_RAND_C relates to mp_rand.c, MP_PRIME_RAND_C relates tp mp_prime_rand.c
   Both use s_mp_rand_source */
#if defined(MP_RAND_C) || defined(MP_PRIME_RAND_C)
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err(*s_mp_rand_source)(void *out, size_t size) = s_mp_rand_platform;

void mp_rand_source(mp_err(*source)(void *out, size_t size))
{
   s_mp_rand_source = (source == NULL) ? s_mp_rand_platform : source;
}
#endif
