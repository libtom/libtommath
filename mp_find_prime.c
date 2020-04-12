#include "tommath_private.h"
#ifdef BN_MP_FIND_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
/* TomsFastMath, a fast ISO C bignum library.
 *
 * This project is public domain and free for all purposes.
 *
 * Love Hornquist Astrand <lha@h5l.org>
 */
#ifdef BN_MP_FIND_PRIME_C
mp_err mp_find_prime(mp_int *a, int t)
{
   bool res = false;

   /* valid value of t? */
   if (t <= 0) {
      return MP_VAL;
   }

   if (mp_iseven(a))
      mp_add_d(a, 1, a);

   do {
      if (mp_prime_is_prime(a, t, &res) != MP_OKAY) {
         res = MP_VAL;
         break;
      }

      if (res == false) {
         mp_add_d(a, 2, a);
         continue;
      }

   } while (res != true);

   return res;
}
#endif
#endif
