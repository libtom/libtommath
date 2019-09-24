#include "tommath_private.h"
#ifdef BN_MP_UBIN_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* get the size for an unsigned equivalent */
size_t mp_ubin_size(const mp_int *a)
{
   int     size = mp_count_bits(a);
   size = (size / 8) + ((((unsigned)size & 7u) != 0u) ? 1 : 0);
   return (size_t)size;
}
#endif
