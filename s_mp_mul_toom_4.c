#include "tommath_private.h"
#ifdef S_MP_MUL_TOOM_4_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/*
   This file contains code from J. Arndt's book  "Matters Computational"
   and the accompanying FXT-library with permission of the author.
*/

/*
    Bodrato, Marco, and Alberto Zanoni. "Integer and polynomial multiplication:
    Towards optimal Toom-Cook matrices." Proceedings of the 2007 international
    symposium on Symbolic and algebraic computation. ACM, 2007.
*/
mp_err s_mp_mul_toom_4(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int S1, S2, S3, S4, S5, S6, S7;
   mp_int tmp1;
   mp_int a0, a1, a2, a3;
   mp_int b0, b1, b2, b3;

   mp_err err = MP_OKAY;
   int B;

   B = MP_MIN(a->used, b->used) / 4;

   if ((err = mp_init_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &tmp1, NULL)) != MP_OKAY) {
      return err;
   }

   /** a = a3*B^3 + a2*B^2 + a1*B + a0; */
   if ((err = s_mp_ro_digs(a,     0, B, &a0)) != MP_OKAY)                                                goto LTM_ERR;
   if ((err = s_mp_ro_digs(a,     B, B, &a1)) != MP_OKAY)                                                goto LTM_ERR;
   if ((err = s_mp_ro_digs(a, 2 * B, B, &a2)) != MP_OKAY)                                                goto LTM_ERR;
   if ((err = s_mp_ro_digs(a, 3 * B, a->used - 3 * B, &a3)) != MP_OKAY)                                  goto LTM_ERR;

   /** b = b3*B^3 + b2*B^2 + b1*B + b0; */
   if ((err = s_mp_ro_digs(b,     0, B, &b0)) != MP_OKAY)                                                goto LTM_ERR;
   if ((err = s_mp_ro_digs(b,     B, B, &b1)) != MP_OKAY)                                                goto LTM_ERR;
   if ((err = s_mp_ro_digs(b, 2 * B, B, &b2)) != MP_OKAY)                                                goto LTM_ERR;
   if ((err = s_mp_ro_digs(b, 3 * B, b->used - 3 * B, &b3)) != MP_OKAY)                                  goto LTM_ERR;



   /** S2 = (8*a3 + 4*a2 + 2*a1 + a0)*(8*b3 + 4*b2 + 2*b1 + b0); */
   if ((err = mp_mul_2d(&a3, 3, &S2)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_mul_2d(&a2, 2, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S2, &tmp1, &S2)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_mul_2d(&a1, 1, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S2, &tmp1, &S2)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S2, &a0, &S2)) != MP_OKAY)                                                          goto LTM_ERR;

   if ((err = mp_mul_2d(&b3, 3, &S3)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_mul_2d(&b2, 2, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S3, &tmp1, &S3)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_mul_2d(&b1, 1, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S3, &tmp1, &S3)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S3, &b0, &S3)) != MP_OKAY)                                                          goto LTM_ERR;

   if ((err = mp_mul(&S2, &S3, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S3 = (+a3 + a2 + a1 + a0)*(+b3 + b2 + b1 + b0); */
   if ((err = mp_add(&a3, &a2, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_add(&a1, &a0, &tmp1)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S3, &tmp1, &S3)) != MP_OKAY)                                                        goto LTM_ERR;

   if ((err = mp_add(&b3, &b2, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_add(&b1, &b0, &tmp1)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S4, &tmp1, &S4)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_mul(&S3, &S4, &S3)) != MP_OKAY)                                                          goto LTM_ERR;

   /** S4 = (-a3+a2-a1+a0)*(-b3+b2-b1+b0); */
   /* = (+a2-a1 + a0-a3)*(+b2-b1 + b0-b3); */

   if ((err = mp_sub(&a2, &a1, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_sub(&a0, &a3, &tmp1)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S4, &tmp1, &S4)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_sub(&b2, &b1, &S5)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_sub(&b0, &b3, &tmp1)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S5, &tmp1, &S5)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_mul(&S4, &S5, &S4)) != MP_OKAY)                                                          goto LTM_ERR;

   /** S5 = (+8*a0+4*a1+2*a2+a3)*(+8*b0+4*b1+2*b2+b3); */
   if ((err = mp_mul_2d(&a0, 3, &S5)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_mul_2d(&a1, 2, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S5, &tmp1, &S5)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_mul_2d(&a2, 1, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S5, &tmp1, &S5)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S5, &a3, &S5)) != MP_OKAY)                                                          goto LTM_ERR;

   if ((err = mp_mul_2d(&b0, 3, &S6)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_mul_2d(&b1, 2, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S6, &tmp1, &S6)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_mul_2d(&b2, 1, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S6, &tmp1, &S6)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S6, &b3, &S6)) != MP_OKAY)                                                          goto LTM_ERR;

   if ((err = mp_mul(&S5, &S6, &S5)) != MP_OKAY)                                                          goto LTM_ERR;

   /** S6 = (-8*a0+4*a1-2*a2+a3)*(-8*b0+4*b1-2*b2+b3); */
   /* = (+4*a1-2*a2+a3-8*a0)*(+4*b1-2*b2+b3-8*b0); */
   if ((err = mp_mul_2d(&a1, 2, &S6)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_mul_2d(&a2, 1, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_sub(&S6, &tmp1, &S6)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S6, &a3, &S6)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_mul_2d(&a0, 3, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_sub(&S6, &tmp1, &S6)) != MP_OKAY)                                                        goto LTM_ERR;

   if ((err = mp_mul_2d(&b1, 2, &S7)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_mul_2d(&b2, 1, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_sub(&S7, &tmp1, &S7)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_add(&S7, &b3, &S7)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_mul_2d(&b0, 3, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_sub(&S7, &tmp1, &S7)) != MP_OKAY)                                                        goto LTM_ERR;
   if ((err = mp_mul(&S6, &S7, &S6)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S7 = a0 * b0; */
   if ((err = mp_mul(&a0, &b0, &S7)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S1 = a3 * b3; */
   if ((err = mp_mul(&a3, &b3, &S1)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S2 = S2 + S5; */
   if ((err = mp_add(&S2, &S5, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S4 = S4 - S3; */
   if ((err = mp_sub(&S4, &S3, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S6 = S6 - S5; */
   if ((err = mp_sub(&S6, &S5, &S6)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S4 = S4 /2 ; */
   if ((err = mp_div_2d(&S4, 1, &S4, NULL)) != MP_OKAY)                                                   goto LTM_ERR;
   /** S5 = S5 - S1; */
   if ((err = mp_sub(&S5, &S1, &S5)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S5 = S5 - (64*S7) ; */
   if ((err = mp_mul_2d(&S7, 6, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_sub(&S5, &tmp1, &S5)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S3 = S3 + S4; */
   if ((err = mp_add(&S3, &S4, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S5 = S5 *2 ; */
   if ((err = mp_mul_2d(&S5, 1, &S5)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S5 = S5 + S6; */
   if ((err = mp_add(&S5, &S6, &S5)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2 - (65*S3) ; */
   if ((err = mp_mul_d(&S3, 65, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_sub(&S2, &tmp1, &S2)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S3 = S3 - S1; */
   if ((err = mp_sub(&S3, &S1, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S3 = S3 - S7; */
   if ((err = mp_sub(&S3, &S7, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S4 = -S4; */
   if ((err = mp_neg(&S4, &S4)) != MP_OKAY)                                                               goto LTM_ERR;
   /** S6 = -S6; */
   if ((err = mp_neg(&S6, &S6)) != MP_OKAY)                                                               goto LTM_ERR;
   /** S2 = S2 + (45*S3) ; */
   if ((err = mp_mul_d(&S3, 45, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S2, &tmp1, &S2)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S5 = S5 - (8*S3) ; */
   if ((err = mp_mul_2d(&S3, 3, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_sub(&S5, &tmp1, &S5)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S5 = S5/24; */
   if ((err = mp_div_d(&S5, 24, &S5, NULL)) != MP_OKAY)                                                   goto LTM_ERR;

   /** S6 = S6 - S2; */
   if ((err = mp_sub(&S6, &S2, &S6)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2 - (16*S4) ; */
   if ((err = mp_mul_2d(&S4, 4, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_sub(&S2, &tmp1, &S2)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S2 = S2/18; */
   if ((err = mp_div_d(&S2, 18, &S2, NULL)) != MP_OKAY)                                                   goto LTM_ERR;
   /** S3 = S3 - S5; */
   if ((err = mp_sub(&S3, &S5, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S4 = S4 - S2; */
   if ((err = mp_sub(&S4, &S2, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S6 = S6 + (30*S2) ; */
   if ((err = mp_mul_d(&S2, 30, &tmp1)) != MP_OKAY)                                                       goto LTM_ERR;
   if ((err = mp_add(&S6, &tmp1, &S6)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S6 = S6/60; */
   if ((err = mp_div_d(&S6, 60, &S6, NULL)) != MP_OKAY)                                                   goto LTM_ERR;
   /** S2 = S2 - S6; */
   if ((err = mp_sub(&S2, &S6, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S = S7 + S6*B + S5*B^2 + S4*B^3 + S3*B^4 + S2*B^5 + S1*B^6; */

   if ((err = mp_copy(&S7, &tmp1)) != MP_OKAY)                                                            goto LTM_ERR;

   if ((err = mp_lshd(&S6, B)) != MP_OKAY)                                                                goto LTM_ERR;
   if ((err = mp_add(&tmp1, &S6, &tmp1)) != MP_OKAY)                                                      goto LTM_ERR;

   if ((err = mp_lshd(&S5, 2 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&tmp1, &S5, &tmp1)) != MP_OKAY)                                                      goto LTM_ERR;

   if ((err = mp_lshd(&S4, 3 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&tmp1, &S4, &tmp1)) != MP_OKAY)                                                      goto LTM_ERR;

   if ((err = mp_lshd(&S3, 4 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&tmp1, &S3, &tmp1)) != MP_OKAY)                                                      goto LTM_ERR;

   if ((err = mp_lshd(&S2, 5 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&tmp1, &S2, &tmp1)) != MP_OKAY)                                                      goto LTM_ERR;

   if ((err = mp_lshd(&S1, 6 * B)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_add(&tmp1, &S1, c)) != MP_OKAY)                                                          goto LTM_ERR;

   /** S - a*b */

LTM_ERR:
   mp_clear_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &tmp1, NULL);
   return err;
}



#endif

