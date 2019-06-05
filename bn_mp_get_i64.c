#include "tommath_private.h"
#ifdef BN_MP_GET_I64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* Return the least significant 64 bits */
int64_t mp_get_i64(const mp_int *a)
{
   uint64_t res = mp_get_mag64(a);
   return (a->sign == MP_NEG) ? (int64_t)-res : (int64_t)res;
}


#endif
