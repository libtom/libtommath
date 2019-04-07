#include "tommath_private.h"
#ifdef BN_MP_JACOBI_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* computes the jacobi c = (a | n) (or Legendre if n is prime)
 * Kept for legacy reasons, please use mp_kronecker() instead
 */
int mp_jacobi(const mp_int *a, const mp_int *n, int *c)
{
   /* if a < 0 return MP_VAL */
   if (a->sign == MP_NEG) {
      return MP_VAL;
   }

   /* if n <= 0 return MP_VAL */
   if (mp_cmp_d(n, 0uL) != MP_GT) {
      return MP_VAL;
   }

   return mp_kronecker(a, n, c);
}
#endif
