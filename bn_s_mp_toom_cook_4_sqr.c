#include "tommath_private.h"
#ifdef BN_S_MP_TOOM_COOK_4_SQR_C
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

   But the large amount of temporary variables could be reduced to one.
*/

int s_mp_toom_cook_4_sqr(const mp_int *a, mp_int *c)
{
   mp_int S1, S2, S3, S4, S5, S6, S7;
   mp_int tmp;
   mp_int a0, a1, a2, a3;

   int e = MP_OKAY;
   int B, count;

   B = (a->used) / 4;

   if ((e = mp_init_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &tmp, NULL)) != MP_OKAY) {
      return e;
   }

   /** A = a3*x^3 + a2*x^2 + a1*x + a0;; */
   if ((e = mp_init_size(&a0, B)) != MP_OKAY) {
      goto LTM_ERRa0;
   }
   for (count = 0; count < B; count++) {
      a0.dp[count] = a->dp[count];
      a0.used++;
   }
   mp_clamp(&a0);
   if ((e = mp_init_size(&a1, B)) != MP_OKAY) {
      goto LTM_ERRa1;
   }
   for (; count < (2 * B); count++) {
      a1.dp[count - B] = a->dp[count];
      a1.used++;
   }
   mp_clamp(&a1);
   if ((e = mp_init_size(&a2, B)) != MP_OKAY) {
      goto LTM_ERRa2;
   }
   for (; count < (3 * B); count++) {
      a2.dp[count - 2 * B] = a->dp[count];
      a2.used++;
   }
   mp_clamp(&a2);
   if ((e = mp_init_size(&a3, B + ((a->used) - 3 * B))) != MP_OKAY) {
      goto LTM_ERRa3;
   }
   for (; count < a->used; count++) {
      a3.dp[count - 3 * B] = a->dp[count];
      a3.used++;
   }
   mp_clamp(&a3);

   /** S2 = a0 * a1; */
   if ((e = mp_mul(&a0, &a1, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 * 2 ; */
   if ((e = mp_mul_2(&S2, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = a0 - a2; */
   if ((e = mp_sub(&a0, &a2, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = a1 - a3; */
   if ((e = mp_sub(&a1, &a3, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S6 + S7; */
   if ((e = mp_add(&S6, &S7, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S6 - S7; */
   if ((e = mp_sub(&S6, &S7, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 * S4; */
   if ((e = mp_mul(&S3, &S4, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S6 * S7; */
   if ((e = mp_mul(&S6, &S7, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 * 2 ; */
   if ((e = mp_mul_2(&S5, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = a0 + a1; */
   if ((e = mp_add(&a0, &a1, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 + a2; */
   if ((e = mp_add(&S4, &a2, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 + a3; */
   if ((e = mp_add(&S4, &a3, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4^2; */
   if ((e = mp_sqr(&S4, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S6 = a3 * a2; */
   if ((e = mp_mul(&a3, &a2, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 * 2 ; */
   if ((e = mp_mul_2(&S6, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** T1 = S3 + S4; */
   if ((e = mp_add(&S3, &S4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** T1 = T1 + S5; */
   if ((e = mp_add(&tmp, &S5, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** T1 = T1 / 2 ; */
   if ((e = mp_div_2(&tmp,&tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S1 = S2 + S6; */
   if ((e = mp_add(&S2, &S6, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = T1 - S1; */
   if ((e = mp_sub(&tmp, &S1, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S1 - S5; */
   if ((e = mp_sub(&S1, &S5, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S7 - S3; */
   if ((e = mp_sub(&S7, &S3, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S1 = a0^2   ; */
   if ((e = mp_sqr(&a0, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S7 - S1; */
   if ((e = mp_sub(&S7, &S1, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = a3^2   ; */
   if ((e = mp_sqr(&a3, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 - S7; */
   if ((e = mp_sub(&S3, &S7, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** P = S7 * x^6 + S6 * x^5 + S5 * x^4 + S4 * x^3 + S3 * x^2 + S2 * x + S1; */
   if ((e = mp_lshd(&S7, 6 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S6, 5 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S7, &S6, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S5, 4 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S7, &S5, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S4, 3 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S7, &S4, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S3, 2 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S7, &S3, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S2, 1 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S7, &S2, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S7, &S1, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** A^2 - P */
   mp_exch(&S7, c);

LTM_ERR:
   mp_clear(&a3);
LTM_ERRa3:
   mp_clear(&a2);
LTM_ERRa2:
   mp_clear(&a1);
LTM_ERRa1:
   mp_clear(&a0);
LTM_ERRa0:
   mp_clear_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &tmp, NULL);
   return e;
}

#endif
