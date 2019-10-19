#include "tommath_private.h"
#ifdef BN_MP_PACK_COUNT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

size_t mp_pack_count(const mp_int *a, size_t nails, size_t size)
{
   size_t bits;
   mp_err err;
   /* TODO: make this function return an mp_err and put the computed value in e.g. size_t *count */
   if ((err = mp_count_bits(a, &bits)) != MP_OKAY) {
      return SIZE_MAX;
   }
   return ((bits / ((size * 8u) - nails)) + (((bits % ((size * 8u) - nails)) != 0u) ? 1u : 0u));
}

#endif
