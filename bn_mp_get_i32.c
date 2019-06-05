#include "tommath_private.h"
#ifdef BN_MP_GET_I32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* Return the least significant 32 bits */
int32_t mp_get_i32(const mp_int *a)
{
   uint64_t res = mp_get_mag32(a);
   return (a->sign == MP_NEG) ? (int32_t)-res : (int32_t)res;
}


#endif
