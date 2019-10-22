#include "tommath_private.h"
#ifdef MP_UBIN_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* get the size for an unsigned equivalent */
size_t mp_ubin_size(const mp_int *a)
{
   unsigned long size = mp_count_bits(a);
   return (size / 8uL) + (((size & 7uL) != 0uL) ? 1uL : 0uL);
}
#endif
