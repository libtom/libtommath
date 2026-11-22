#include "tommath_private.h"
#ifdef S_MP_SQR_TOOM_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* squaring using Toom-Cook 3-way algorithm */

/*
   This file contains code from J. Arndt's book  "Matters Computational"
   and the accompanying FXT-library with permission of the author.
*/

/* squaring using Toom-Cook 3-way algorithm */
/*
   Setup and interpolation from algorithm SQR_3 in

     Chung, Jaewook, and M. Anwar Hasan. "Asymmetric squaring formulae."
     18th IEEE Symposium on Computer Arithmetic (ARITH'07). IEEE, 2007.

*/
mp_err s_mp_sqr_toom(const mp_int *a, mp_int *b)
{
   mp_int S0, S1, S2, S3, S4, tmp, a0, a1, a2;
   int B;
   mp_err err;

   /* init temps */
   if ((err = mp_init_multi(&S0, &S1, &S2, &S3, &S4, &tmp, NULL)) != MP_OKAY) {
      return err;
   }

   /* B */
   B = a->used / 3;

   /** a = a2*x^2 + a1*x + a0; */
   if ((err = s_mp_ro_digs(a,     0, B, &a0)) != MP_OKAY)                                                 goto LBL_ERR;
   if ((err = s_mp_ro_digs(a,     B, B, &a1)) != MP_OKAY)                                                 goto LBL_ERR;
   if ((err = s_mp_ro_digs(a, 2 * B, a->used - 2 * B, &a2)) != MP_OKAY)                                   goto LBL_ERR;


   /** S1 = a0 + a2; */
   if ((err = mp_add(&a0, &a2, &S1)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S2 = S1 - a1; */
   if ((err = mp_sub(&S1, &a1, &S2)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S1 = S1 + a1; */
   if ((err = mp_add(&S1, &a1, &S1)) != MP_OKAY)                                                          goto LBL_ERR;

   /** S1 = S1^2;  */
   if ((err = mp_sqr(&S1, &S1)) != MP_OKAY)                                                               goto LBL_ERR;
   /** S2 = S2^2;  */
   if ((err = mp_sqr(&S2, &S2)) != MP_OKAY)                                                               goto LBL_ERR;

   /** S0 = a0^2;  */
   if ((err = mp_sqr(&a0, &S0)) != MP_OKAY)                                                               goto LBL_ERR;
   /** S4 = a2^2;  */
   if ((err = mp_sqr(&a2, &S4)) != MP_OKAY)                                                               goto LBL_ERR;
   /** S3 = a1 * a2;  */
   if ((err = mp_mul(&a1, &a2, &S3)) != MP_OKAY)                                                          goto LBL_ERR;

   /** S3 = S3 << 1;  */
   if ((err = mp_mul_2(&S3, &S3)) != MP_OKAY)                                                             goto LBL_ERR;

   /** tmp = S1 + S2; */
   if ((err = mp_add(&S1, &S2, &tmp)) != MP_OKAY)                                                          goto LBL_ERR;
   /** tmp = tmp >> 1; */
   if ((err = mp_div_2(&tmp, &tmp)) != MP_OKAY)                                                           goto LBL_ERR;
   /** S1 = S1 - tmp; */
   if ((err = mp_sub(&S1, &tmp, &S1)) != MP_OKAY)                                                         goto LBL_ERR;
   /** S1 = S1 - S3;  */
   if ((err = mp_sub(&S1, &S3, &S1)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S2 = tmp - S4;  */
   if ((err = mp_sub(&tmp, &S4, &S2)) != MP_OKAY)                                                         goto LBL_ERR;
   /** S2 = S2 - S0;  */
   if ((err = mp_sub(&S2, &S0, &S2)) != MP_OKAY)                                                          goto LBL_ERR;

   /** P = S4*x^4 + S3*x^3 + S2*x^2 + S1*x + S0; */
   if ((err = mp_lshd(&S1, 1 * B)) != MP_OKAY)                                                            goto LBL_ERR;
   if ((err = mp_add(&S0, &S1, &S0)) != MP_OKAY)                                                          goto LBL_ERR;
   if ((err = mp_lshd(&S2, 2 * B)) != MP_OKAY)                                                            goto LBL_ERR;
   if ((err = mp_add(&S0, &S2, &S0)) != MP_OKAY)                                                          goto LBL_ERR;
   if ((err = mp_lshd(&S3, 3 * B)) != MP_OKAY)                                                            goto LBL_ERR;
   if ((err = mp_add(&S0, &S3, &S0)) != MP_OKAY)                                                          goto LBL_ERR;
   if ((err = mp_lshd(&S4, 4 * B)) != MP_OKAY)                                                            goto LBL_ERR;
   if ((err = mp_add(&S0, &S4, &S0)) != MP_OKAY)                                                          goto LBL_ERR;
   /** a^2 - P  */

   mp_exch(&S0, b);

LBL_ERR:
   mp_clear_multi(&S0, &S1, &S2, &S3, &S4, &tmp, NULL);

   return err;
}

#endif
