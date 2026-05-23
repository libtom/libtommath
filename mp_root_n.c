#include "tommath_private.h"
#ifdef MP_ROOT_N_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* find the n'th root of an integer
 *
 * Result found such that (c)**b <= a and (c+1)**b > a
 */

mp_err mp_root_n(const mp_int *a, int b, mp_int *c)
{
   return s_mp_root_n(a, b, c, NULL);
}

#endif
