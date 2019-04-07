#include "tommath_private.h"
#ifdef BN_MP_EXPT_D_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* wrapper function for mp_expt_d_ex() */
int mp_expt_d(const mp_int *a, mp_digit b, mp_int *c)
{
   return mp_expt_d_ex(a, b, c, 0);
}

#endif
