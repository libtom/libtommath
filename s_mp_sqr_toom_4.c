#include "tommath_private.h"
#ifdef S_MP_SQR_TOOM_4_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/*
   This file contains code from J. Arndt's book  "Matters Computational"
   and the accompanying FXT-library with permission of the author.
*/

/*
   Setup and interpolation from

     Chung, Jaewook, and M. Anwar Hasan. "Asymmetric squaring formulae."
     18th IEEE Symposium on Computer Arithmetic (ARITH'07). IEEE, 2007.
*/
mp_err s_mp_sqr_toom_4(const mp_int *a, mp_int *c)
{
   mp_int S1, S2, S3, S4, S5, S6, S7;
   mp_int tmp;
   mp_int a0, a1, a2, a3;

   mp_err err = MP_OKAY;
   int B;

   B = (a->used) / 4;

   if ((err = mp_init_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &tmp, NULL)) != MP_OKAY) {
      return err;
   }


   /** A = a3*x^3 + a2*x^2 + a1*x + a0; */

   if ((err = s_mp_ro_digs(a,     0, B, &a0)) != MP_OKAY)                                                goto LTM_ERR;
   if ((err = s_mp_ro_digs(a,     B, B, &a1)) != MP_OKAY)                                                goto LTM_ERR;
   if ((err = s_mp_ro_digs(a, 2 * B, B, &a2)) != MP_OKAY)                                                goto LTM_ERR;
   if ((err = s_mp_ro_digs(a, 3 * B, a->used - 3 * B, &a3)) != MP_OKAY)                                  goto LTM_ERR;

   /** S2 = a0 * a1; */
   if ((err = mp_mul(&a0, &a1, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2 * 2 ; */
   if ((err = mp_mul_2(&S2, &S2)) != MP_OKAY)                                                             goto LTM_ERR;
   /** S6 = a0 - a2; */
   if ((err = mp_sub(&a0, &a2, &S6)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S7 = a1 - a3; */
   if ((err = mp_sub(&a1, &a3, &S7)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S3 = S6 + S7; */
   if ((err = mp_add(&S6, &S7, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S4 = S6 - S7; */
   if ((err = mp_sub(&S6, &S7, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S3 = S3 * S4; */
   if ((err = mp_mul(&S3, &S4, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S5 = S6 * S7; */
   if ((err = mp_mul(&S6, &S7, &S5)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S5 = S5 * 2 ; */
   if ((err = mp_mul_2(&S5, &S5)) != MP_OKAY)                                                             goto LTM_ERR;
   /** S4 = a0 + a1; */
   if ((err = mp_add(&a0, &a1, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S4 = S4 + a2; */
   if ((err = mp_add(&S4, &a2, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S4 = S4 + a3; */
   if ((err = mp_add(&S4, &a3, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S4 = S4^2; */
   if ((err = mp_sqr(&S4, &S4)) != MP_OKAY)                                                               goto LTM_ERR;

   /** S6 = a3 * a2; */
   if ((err = mp_mul(&a3, &a2, &S6)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S6 = S6 * 2 ; */
   if ((err = mp_mul_2(&S6, &S6)) != MP_OKAY)                                                             goto LTM_ERR;
   /** T1 = S3 + S4; */
   if ((err = mp_add(&S3, &S4, &tmp)) != MP_OKAY)                                                         goto LTM_ERR;
   /** T1 = T1 + S5; */
   if ((err = mp_add(&tmp, &S5, &tmp)) != MP_OKAY)                                                        goto LTM_ERR;
   /** T1 = T1 / 2 ; */
   if ((err = mp_div_2(&tmp,&tmp)) != MP_OKAY)                                                            goto LTM_ERR;
   /** S1 = S2 + S6; */
   if ((err = mp_add(&S2, &S6, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S7 = T1 - S1; */
   if ((err = mp_sub(&tmp, &S1, &tmp)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S4 = S1 - S5; */
   if ((err = mp_sub(&S1, &S5, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S3 = S7 - S3; */
   if ((err = mp_sub(&tmp, &S3, &S3)) != MP_OKAY)                                                         goto LTM_ERR;

   /** S1 = a0^2   ; */
   if ((err = mp_sqr(&a0, &S1)) != MP_OKAY)                                                               goto LTM_ERR;
   /** S7 = a3^2   ; */
   if ((err = mp_sqr(&a3, &S7)) != MP_OKAY)                                                               goto LTM_ERR;

   /** S5 = S7 - S1; */
   if ((err = mp_sub(&tmp, &S1, &S5)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S3 = S3 - S7; */
   if ((err = mp_sub(&S3, &S7, &S3)) != MP_OKAY)                                                          goto LTM_ERR;

   /** P = S7 * x^6 + S6 * x^5 + S5 * x^4 + S4 * x^3 + S3 * x^2 + S2 * x + S1; */
   if ((err = mp_lshd(&S7, 6 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_lshd(&S6, 5 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&S7, &S6, &S7)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S5, 4 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&S7, &S5, &S7)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S4, 3 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&S7, &S4, &S7)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S3, 2 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&S7, &S3, &S7)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S2, 1 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&S7, &S2, &S7)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_add(&S7, &S1, &S7)) != MP_OKAY)                                                          goto LTM_ERR;

   /** A^2 - P */
   mp_exch(&S7, c);
LTM_ERR:
   mp_clear_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &tmp, NULL);
   return err;
}



#endif

