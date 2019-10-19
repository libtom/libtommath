#include "tommath_private.h"
#ifdef BN_MP_UBIN_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* get the size for an unsigned equivalent */
size_t mp_ubin_size(const mp_int *a)
{
   size_t size;
   mp_err err;
   /* TODO: make this function return an mp_err and put the result in e.g.: size_t size*/
   if ((err = mp_count_bits(a, &size)) != MP_OKAY) {
      /* Nothing, but see TODO above */
   }
   return (size / 8u) + (((size & 7u) != 0u) ? 1u : 0u);
}
#endif
