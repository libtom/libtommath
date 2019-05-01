#include "tommath_private.h"
#ifdef BN_S_MP_TOOM_COOK_4_MUL_C
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
int s_mp_toom_cook_4_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int S1, S2, S3, S4, S5, S6, S7;
   mp_int tmp1;
   mp_int a0, a1, a2, a3;
   mp_int b0, b1, b2, b3;

   int e = MP_OKAY;
   int B, count, j;

   B = MP_MIN(a->used, b->used) / 4;

   if ((e = mp_init_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &tmp1, NULL)) != MP_OKAY) {
      return e;
   }

   /** a = a3*B^3 + a2*B^2 + a1*B + a0; */
   if ((e = mp_init_size(&a0, B)) != MP_OKAY) {
      goto LTM_ERRa0;
   }
   for (j = 0, count = 0; count < B; count++, j++) {
      a0.dp[j] = a->dp[count];
      a0.used++;
   }
   mp_clamp(&a0);
   if ((e = mp_init_size(&a1, B)) != MP_OKAY) {
      goto LTM_ERRa1;
   }
   for (j = 0; count < 2 * B; count++, j++) {
      a1.dp[j] = a->dp[count];
      a1.used++;
   }
   mp_clamp(&a1);
   if ((e = mp_init_size(&a2, B)) != MP_OKAY) {
      goto LTM_ERRa2;
   }
   for (j = 0; count < 3 * B; count++, j++) {
      a2.dp[j] = a->dp[count];
      a2.used++;
   }
   mp_clamp(&a2);
   if ((e = mp_init_size(&a3, B + (a->used - 3 * B))) != MP_OKAY) {
      goto LTM_ERRa3;
   }
   for (j = 0; count < a->used; count++, j++) {
      a3.dp[j] = a->dp[count];
      a3.used++;
   }
   mp_clamp(&a3);

   /** b = b3*B^3 + b2*B^2 + b1*B + b0; */
   if ((e = mp_init_size(&b0, B)) != MP_OKAY) {
      goto LTM_ERRb0;
   }
   for (j = 0, count = 0; count < B; count++, j++) {
      b0.dp[j] = b->dp[count];
      b0.used++;
   }
   mp_clamp(&b0);
   if ((e = mp_init_size(&b1, B)) != MP_OKAY) {
      goto LTM_ERRb1;
   }
   for (j = 0; count < 2 * B; count++, j++) {
      b1.dp[j] = b->dp[count];
      b1.used++;
   }
   mp_clamp(&b1);
   if ((e = mp_init_size(&b2, B)) != MP_OKAY) {
      goto LTM_ERRb2;
   }
   for (j = 0; count < 3 * B; count++, j++) {
      b2.dp[j] = b->dp[count];
      b2.used++;
   }
   mp_clamp(&b2);
   if ((e = mp_init_size(&b3, B + (b->used - 3 * B))) != MP_OKAY) {
      goto LTM_ERRb3;
   }
   for (j = 0; count < b->used; count++, j++) {
      b3.dp[j] = b->dp[count];
      b3.used++;
   }
   mp_clamp(&b3);


   /** S1 = a3 * b3; */
   if ((e = mp_mul(&a3, &b3, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = (8*a3 + 4*a2 + 2*a1 + a0)*(8*b3 + 4*b2 + 2*b1 + b0); */
   if ((e = mp_mul_2d(&a3, 3, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&a2, 2, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S2, &tmp1, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&a1, 1, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S2, &tmp1, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S2, &a0, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_mul_2d(&b3, 3, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&b2, 2, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S3, &tmp1, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&b1, 1, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S3, &tmp1, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S3, &b0, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_mul(&S2, &S3, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = (+a3 + a2 + a1 + a0)*(+b3 + b2 + b1 + b0); */
   if ((e = mp_add(&a3, &a2, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&a1, &a0, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S3, &tmp1, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_add(&b3, &b2, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&b1, &b0, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S4, &tmp1, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul(&S3, &S4, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S4 = (-a3+a2-a1+a0)*(-b3+b2-b1+b0); */
   /* = (+a2-a1 + a0-a3)*(+b2-b1 + b0-b3); */

   if ((e = mp_sub(&a2, &a1, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&a0, &a3, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S4, &tmp1, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&b2, &b1, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&b0, &b3, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S5, &tmp1, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul(&S4, &S5, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S5 = (+8*a0+4*a1+2*a2+a3)*(+8*b0+4*b1+2*b2+b3); */
   if ((e = mp_mul_2d(&a0, 3, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&a1, 2, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S5, &tmp1, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&a2, 1, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S5, &tmp1, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S5, &a3, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_mul_2d(&b0, 3, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&b1, 2, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S6, &tmp1, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&b2, 1, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S6, &tmp1, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S6, &b3, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_mul(&S5, &S6, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S6 = (-8*a0+4*a1-2*a2+a3)*(-8*b0+4*b1-2*b2+b3); */
   /* = (+4*a1-2*a2+a3-8*a0)*(+4*b1-2*b2+b3-8*b0); */
   if ((e = mp_mul_2d(&a1, 2, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&a2, 1, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&S6, &tmp1, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S6, &a3, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&a0, 3, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&S6, &tmp1, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_mul_2d(&b1, 2, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&b2, 1, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&S7, &tmp1, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S7, &b3, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&b0, 3, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&S7, &tmp1, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul(&S6, &S7, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = a0 * b0; */
   if ((e = mp_mul(&a0, &b0, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 + S5; */
   if ((e = mp_add(&S2, &S5, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 - S3; */
   if ((e = mp_sub(&S4, &S3, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 - S5; */
   if ((e = mp_sub(&S6, &S5, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 /2 ; */
   if ((e = mp_div_2d(&S4, 1, &S4, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 - S1; */
   if ((e = mp_sub(&S5, &S1, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 - (64*S7) ; */
   if ((e = mp_mul_2d(&S7, 6, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&S5, &tmp1, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + S4; */
   if ((e = mp_add(&S3, &S4, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 *2 ; */
   if ((e = mp_mul_2d(&S5, 1, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 + S6; */
   if ((e = mp_add(&S5, &S6, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 - (65*S3) ; */
   if ((e = mp_mul_d(&S3, 65, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&S2, &tmp1, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 - S1; */
   if ((e = mp_sub(&S3, &S1, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 - S7; */
   if ((e = mp_sub(&S3, &S7, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = -S4; */
   if ((e = mp_neg(&S4, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = -S6; */
   if ((e = mp_neg(&S6, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 + (45*S3) ; */
   if ((e = mp_mul_d(&S3, 45, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S2, &tmp1, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 - (8*S3) ; */
   if ((e = mp_mul_2d(&S3, 3, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&S5, &tmp1, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5/24; */
   if ((e = mp_div_d(&S5, 24, &S5, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S6 = S6 - S2; */
   if ((e = mp_sub(&S6, &S2, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 - (16*S4) ; */
   if ((e = mp_mul_2d(&S4, 4, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_sub(&S2, &tmp1, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2/18; */
   if ((e = mp_div_d(&S2, 18, &S2, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 - S5; */
   if ((e = mp_sub(&S3, &S5, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 - S2; */
   if ((e = mp_sub(&S4, &S2, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 + (30*S2) ; */
   if ((e = mp_mul_d(&S2, 30, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&S6, &tmp1, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6/60; */
   if ((e = mp_div_d(&S6, 60, &S6, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 - S6; */
   if ((e = mp_sub(&S2, &S6, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S = S7 + S6*B + S5*B^2 + S4*B^3 + S3*B^4 + S2*B^5 + S1*B^6; */

   if ((e = mp_copy(&S7, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S6, B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp1, &S6, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S5, 2 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp1, &S5, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S4, 3 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp1, &S4, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S3, 4 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp1, &S3, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S2, 5 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp1, &S2, &tmp1)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S1, 6 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp1, &S1, c)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S - a*b */

LTM_ERR:
   mp_clear(&b3);
LTM_ERRb3:
   mp_clear(&b2);
LTM_ERRb2:
   mp_clear(&b1);
LTM_ERRb1:
   mp_clear(&b0);
LTM_ERRb0:
   mp_clear(&a3);
LTM_ERRa3:
   mp_clear(&a2);
LTM_ERRa2:
   mp_clear(&a1);
LTM_ERRa1:
   mp_clear(&a0);
LTM_ERRa0:
   mp_clear_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &tmp1, NULL);
   return e;
}
#endif

