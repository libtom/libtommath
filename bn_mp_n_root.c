#include "tommath_private.h"
#ifdef BN_MP_N_ROOT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* wrapper function for mp_n_root_ex()
 * computes c = (a)**(1/b) such that (c)**b <= a and (c+1)**b > a
 */
int mp_n_root(const mp_int *a, mp_digit b, mp_int *c)
{
   return mp_n_root_ex(a, b, c, 0);
}

#endif
