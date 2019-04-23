#include "tommath_private.h"
#ifdef BN_S_MP_TOOM_COOK_5_MUL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/*
    Bodrato, Marco, and Alberto Zanoni. "Integer and polynomial multiplication:
    Towards optimal Toom-Cook matrices." Proceedings of the 2007 international
    symposium on Symbolic and algebraic computation. ACM, 2007.
*/

int s_mp_toom_cook_5_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int S1, S2, S3, S4, S5, S6, S7, S8, S9;
   mp_int tmp, tmp2;
   mp_int a0, a1, a2, a3, a4;
   mp_int b0, b1, b2, b3, b4;
   int e = MP_OKAY;
   int B, count, j, sign;

   B = MP_MIN(a->used, b->used) / 5;
   if ((e =
           mp_init_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &S8, &S9, &tmp, &tmp2, NULL)) != MP_OKAY) {
      return e;
   }

   /** A = a4*x^4 + a3*x^3 + a2*x^2 + a1*x + a0 */
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

   if ((e = mp_init_size(&a3, B)) != MP_OKAY) {
      goto LTM_ERRa3;
   }
   for (j = 0; count < 4 * B; count++, j++) {
      a3.dp[j] = a->dp[count];
      a3.used++;
   }
   mp_clamp(&a3);

   if ((e = mp_init_size(&a4, B + (a->used - 4 * B))) != MP_OKAY) {
      goto LTM_ERRa4;
   }
   for (j = 0; count < a->used; count++, j++) {
      a4.dp[j] = a->dp[count];
      a4.used++;
   }
   mp_clamp(&a4);

   /** B = b4*x^4 + b3*x^3 + b2*x^2 + b1*x + b0 */
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

   if ((e = mp_init_size(&b3, B)) != MP_OKAY) {
      goto LTM_ERRb3;
   }
   for (j = 0; count < 4 * B; count++, j++) {
      b3.dp[j] = b->dp[count];
      b3.used++;
   }
   mp_clamp(&b3);

   if ((e = mp_init_size(&b4, B + (b->used - 4 * B))) != MP_OKAY) {
      goto LTM_ERRb4;
   }
   for (j = 0; count < b->used; count++, j++) {
      b4.dp[j] = b->dp[count];
      b4.used++;
   }
   mp_clamp(&b4);

   /** S1 = a4*b4 */
   if ((e = mp_mul(&a4, &b4, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S2 = (a0-2*a1+4*a2-8*a3+16*a4)*(b0-2*b1+4*b2-8*b3+16*b4) */
   /** tmp = a1 << 1 */
   if ((e = mp_mul_2(&a1, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = a0 - tmp */
   if ((e = mp_sub(&a0, &tmp, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a2 << 2 */
   if ((e = mp_mul_2d(&a2, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 + tmp */
   if ((e = mp_add(&S2, &tmp, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a3 << 3 */
   if ((e = mp_mul_2d(&a3, 3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 - tmp */
   if ((e = mp_sub(&S2, &tmp, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a4 << 4 */
   if ((e = mp_mul_2d(&a4, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 + tmp */
   if ((e = mp_add(&S2, &tmp, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b1 << 1 */
   if ((e = mp_mul_2(&b1, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = b0 - tmp */
   if ((e = mp_sub(&b0, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b2 << 2 */
   if ((e = mp_mul_2d(&b2, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 + tmp */
   if ((e = mp_add(&S5, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b3 << 3 */
   if ((e = mp_mul_2d(&b3, 3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 - tmp */
   if ((e = mp_sub(&S5, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b4 << 4 */
   if ((e = mp_mul_2d(&b4, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 + tmp */
   if ((e = mp_add(&S5, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 * S5 */
   if ((e = mp_mul(&S2, &S5, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S5 = (a0+2*a1+4*a2+8*a3+16*a4)*(b0+2*b1+4*b2+8*b3+16*b4) */
   /** tmp = a1 << 1 */
   if ((e = mp_mul_2(&a1, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = a0 + tmp */
   if ((e = mp_add(&a0, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a2 << 2 */
   if ((e = mp_mul_2d(&a2, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 + tmp */
   if ((e = mp_add(&S5, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a3 << 3 */
   if ((e = mp_mul_2d(&a3, 3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 + tmp */
   if ((e = mp_add(&S5, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a4 << 4 */
   if ((e = mp_mul_2d(&a4, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 + tmp */
   if ((e = mp_add(&S5, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b1 << 1 */
   if ((e = mp_mul_2(&b1, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = b0 + tmp */
   if ((e = mp_add(&b0, &tmp, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b2 << 2 */
   if ((e = mp_mul_2d(&b2, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + tmp */
   if ((e = mp_add(&S3, &tmp, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b3 << 3 */
   if ((e = mp_mul_2d(&b3, 3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + tmp */
   if ((e = mp_add(&S3, &tmp, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b4 << 4 */
   if ((e = mp_mul_2d(&b4, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + tmp */
   if ((e = mp_add(&S3, &tmp, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 * S3 */
   if ((e = mp_mul(&S5, &S3, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S3 = (a4+2*a3+4*a2+8*a1+16*a0)*(b4+2*b3+4*b2+8*b1+16*b0) */

   /** tmp = a3 << 1 */
   if ((e = mp_mul_2(&a3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = a4 + tmp */
   if ((e = mp_add(&a4, &tmp, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a2 << 2 */
   if ((e = mp_mul_2d(&a2, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + tmp */
   if ((e = mp_add(&S3, &tmp, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a1 << 3 */
   if ((e = mp_mul_2d(&a1, 3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + tmp */
   if ((e = mp_add(&S3, &tmp, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a0 << 4 */
   if ((e = mp_mul_2d(&a0, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + tmp */
   if ((e = mp_add(&S3, &tmp, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b3 << 1 */
   if ((e = mp_mul_2(&b3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = b4 + tmp */
   if ((e = mp_add(&b4, &tmp, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b2 << 2 */
   if ((e = mp_mul_2d(&b2, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 + tmp */
   if ((e = mp_add(&S8, &tmp, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b1 << 3 */
   if ((e = mp_mul_2d(&b1, 3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 + tmp */
   if ((e = mp_add(&S8, &tmp, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b0 << 4 */
   if ((e = mp_mul_2d(&b0, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 + tmp */
   if ((e = mp_add(&S8, &tmp, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 * S8 */
   if ((e = mp_mul(&S3, &S8, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = (a4-2*a3+4*a2-8*a1+16*a0)*(b4-2*b3+4*b2-8*b1+16*b0) */
   /** tmp = a3 << 1 */
   if ((e = mp_mul_2(&a3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = a4 - tmp */
   if ((e = mp_sub(&a4, &tmp, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a2 << 2 */
   if ((e = mp_mul_2d(&a2, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 + tmp */
   if ((e = mp_add(&S8, &tmp, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a1 << 3 */
   if ((e = mp_mul_2d(&a1, 3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 - tmp */
   if ((e = mp_sub(&S8, &tmp, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a0 << 4 */
   if ((e = mp_mul_2d(&a0, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 + tmp */
   if ((e = mp_add(&S8, &tmp, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b3 << 1 */
   if ((e = mp_mul_2(&b3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = b4 - tmp */
   if ((e = mp_sub(&b4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b2 << 2 */
   if ((e = mp_mul_2d(&b2, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 + tmp */
   if ((e = mp_add(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b1 << 3 */
   if ((e = mp_mul_2d(&b1, 3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 - tmp */
   if ((e = mp_sub(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b0 << 4 */
   if ((e = mp_mul_2d(&b0, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 + tmp */
   if ((e = mp_add(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 * S4 */
   if ((e = mp_mul(&S8, &S4, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S4 = (a0+4*a1+16*a2+64*a3+256*a4)*(b0+4*b1+16*b2+64*b3+256*b4) */
   /** tmp = a1 << 2 */
   if ((e = mp_mul_2d(&a1, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = a0 + tmp */
   if ((e = mp_add(&a0, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a2 << 4 */
   if ((e = mp_mul_2d(&a2, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 + tmp */
   if ((e = mp_add(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a3 << 6 */
   if ((e = mp_mul_2d(&a3, 6, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 + tmp */
   if ((e = mp_add(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = a4 << 8 */
   if ((e = mp_mul_2d(&a4, 8, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 + tmp */
   if ((e = mp_add(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** tmp = b1 << 2 */
   if ((e = mp_mul_2d(&b1, 2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = b0 + tmp */
   if ((e = mp_add(&b0, &tmp, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b2 << 4 */
   if ((e = mp_mul_2d(&b2, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 + tmp */
   if ((e = mp_add(&S6, &tmp, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b3 << 6 */
   if ((e = mp_mul_2d(&b3, 6, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 + tmp */
   if ((e = mp_add(&S6, &tmp, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = b4 << 8 */
   if ((e = mp_mul_2d(&b4, 8, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 + tmp */
   if ((e = mp_add(&S6, &tmp, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 * S6 */
   if ((e = mp_mul(&S4, &S6, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S6 = (a0-a1+a2-a3+a4)*(b0-b1+b2-b3+b4) */
   /** S6 = a0 - a1 */
   if ((e = mp_sub(&a0, &a1, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 + a2 */
   if ((e = mp_add(&S6, &a2, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 - a3 */
   if ((e = mp_sub(&S6, &a3, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 + a4 */
   if ((e = mp_add(&S6, &a4, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = b0 - b1 */
   if ((e = mp_sub(&b0, &b1, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 + b2 */
   if ((e = mp_add(&S7, &b2, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 - b3 */
   if ((e = mp_sub(&S7, &b3, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 + b4 */
   if ((e = mp_add(&S7, &b4, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 * S7 */
   if ((e = mp_mul(&S6, &S7, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** \\S7 = (a0+a1+a2+a3+a4)*(b0+b1+b2+b3+b4) */
   /** S7 = a0 + a1 */
   if ((e = mp_add(&a0, &a1, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 + a2 */
   if ((e = mp_add(&S7, &a2, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 + a3 */
   if ((e = mp_add(&S7, &a3, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 + a4 */
   if ((e = mp_add(&S7, &a4, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S9 = b0 + b1 */
   if ((e = mp_add(&b0, &b1, &S9)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S9 = S9 + b2 */
   if ((e = mp_add(&S9, &b2, &S9)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S9 = S9 + b3 */
   if ((e = mp_add(&S9, &b3, &S9)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S9 = S9 + b4 */
   if ((e = mp_add(&S9, &b4, &S9)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 * S9 */
   if ((e = mp_mul(&S7, &S9, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S9 = a0*b0 */
   if ((e = mp_mul(&a0, &b0, &S9)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 - S7 */
   if ((e = mp_sub(&S6, &S7, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 - S5 */
   if ((e = mp_sub(&S2, &S5, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 - S9 */
   if ((e = mp_sub(&S4, &S9, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S4 = S4 - (2^16*S1) */
   /** tmp = S1 << 16 */
   if ((e = mp_mul_2d(&S1, 16, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 - tmp */
   if ((e = mp_sub(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 - S3 */
   if ((e = mp_sub(&S8, &S3, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 / 2 */
   if ((e = mp_div_2(&S6, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 * 2 */
   if ((e = mp_mul_2(&S5, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 + S2 */
   if ((e = mp_add(&S5, &S2, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = -S2 */
   if ((e = mp_neg(&S2, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = -S8 */
   if ((e = mp_neg(&S8, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 + S6 */
   if ((e = mp_add(&S7, &S6, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = -S6 */
   if ((e = mp_neg(&S6, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 - S7 */
   if ((e = mp_sub(&S3, &S7, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S5 = S5 - (S7 * 512) */
   /** tmp = S7 << 9 */
   if ((e = mp_mul_2d(&S7, 9, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 - tmp */
   if ((e = mp_sub(&S5, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S3 = S3 * 2 */
   if ((e = mp_mul_2(&S3, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 - S8 */
   if ((e = mp_sub(&S3, &S8, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 - S1 */
   if ((e = mp_sub(&S7, &S1, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 - S9 */
   if ((e = mp_sub(&S7, &S9, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 + S2 */
   if ((e = mp_add(&S8, &S2, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 + S3 */
   if ((e = mp_add(&S5, &S3, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S8 = S8 - (S6 * 80) */
   /** tmp = S6 * 80 */
   if ((e = mp_mul_d(&S6, (mp_digit)80, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 - tmp */
   if ((e = mp_sub(&S8, &tmp, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S3 = S3 - (S9 * 510) */
   /** tmp = S9 * 510 */
#ifdef MP_8BIT
   if ((e = mp_mul_d(&S9, 85, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_d(&tmp, 3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2(&tmp, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
#else
   if ((e = mp_mul_d(&S9, (mp_digit)510, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
#endif
   /** S3 = S3 - tmp */
   if ((e = mp_sub(&S3, &tmp, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 - S2 */
   if ((e = mp_sub(&S4, &S2, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 * 3 */
   if ((e = mp_mul_d(&S3, (mp_digit)3, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + S5 */
   if ((e = mp_add(&S3, &S5, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 / 180 */
#ifdef MP_8BIT
   /* multi-limb division is way too slow, as expected */
   /*
      if ((e = mp_set_long(&tmp, 180uL)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((e = mp_div(&S8, &tmp, &S8, NULL)) != MP_OKAY) {
         goto LTM_ERR;
      }
   */
   /* 180 = 4 * 45 */
   if ((e = mp_div_2d(&S8, 2, &S8, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_div_d(&S8, 45, &S8, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
#else
   if ((e = mp_div_d(&S8, (mp_digit)180, &S8, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
#endif
   /** \\S5 = S5 + (S7 * 378) */
   /** tmp = S7 * 378 */
#ifdef MP_8BIT
   if ((e = mp_mul_d(&S7, 27, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_d(&tmp, 7, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2(&tmp, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
#else
   if ((e = mp_mul_d(&S7, (mp_digit)378, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
#endif
   /** S5  = S5 + tmp */
   if ((e = mp_add(&S5, &tmp, &S5)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 =  S2 >> 2 */
   if ((e = mp_div_2d(&S2, 2, &S2, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 - S2 */
   if ((e = mp_sub(&S6, &S2, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S5 = S5 / (-72) */
   sign = S5.sign;
   if ((e = mp_div_d(&S5, 72, &S5, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   S5.sign = (sign == MP_NEG)?MP_ZPOS: MP_NEG;
   /** S3 = S3 / (-360) */
   sign = S3.sign;
#ifdef MP_8BIT
   /*
      if ((e = mp_set_long(&tmp, 360uL)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((e = mp_neg(&tmp, &tmp)) != MP_OKAY) {
         goto LTM_ERR;
      }

      if ((e = mp_div(&S3, &tmp, &S3, NULL)) != MP_OKAY) {
         goto LTM_ERR;
      }
   */
   /* 360 = 8 * 45 */
   if ((e = mp_div_2d(&S3, 3, &S3, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_div_d(&S3, 45, &S3, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
#else
   if ((e = mp_div_d(&S3, (mp_digit)360, &S3, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
#endif
   S3.sign = (sign == MP_NEG)?MP_ZPOS: MP_NEG;
   /** S2 = S2 - S8 */
   if ((e = mp_sub(&S2, &S8, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S7 = S7 - S3 */
   if ((e = mp_sub(&S7, &S3, &S7)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S4 = S4 - (S5 * 256) */
   /** tmp = S5 << 8 */
   if ((e = mp_mul_2d(&S5, 8, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 - tmp */
   if ((e = mp_sub(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 - S5 */
   if ((e = mp_sub(&S3, &S5, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S4 = S4 - (S3 * 4096) */
   /** tmp = S3 << 12 */
   if ((e = mp_mul_2d(&S3, 12, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 - tmp */
   if ((e = mp_sub(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S4 = S4 - (S7 * 16) */
   /** tmp = S7 << 4 */
   if ((e = mp_mul_2d(&S7, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 - tmp */
   if ((e = mp_sub(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S4 = S4 + (S6 * 256) */
   /** tmp = S6 << 8 */
   if ((e = mp_mul_2d(&S6, 8, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 + tmp */
   if ((e = mp_add(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S6 = S6 + S2 */
   if ((e = mp_add(&S6, &S2, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 * 180 */
#ifdef MP_8BIT
   if ((e = mp_mul_d(&S2, 45, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&S2, 2, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
#else
   if ((e = mp_mul_d(&S2, (mp_digit)180, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
#endif
   /** S2 = S2 + S4 */
   if ((e = mp_add(&S2, &S4, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 / 11340 */
#ifdef MP_8BIT
   /*
      if ((e = mp_set_long(&tmp, 11340uL)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((e = mp_div(&S2, &tmp, &S2, NULL)) != MP_OKAY) {
         goto LTM_ERR;
      }
   */
   /* 11340 = 2^2 * 3^4 * 5 * 7 = 2^2 * 81 * 35 */
   if ((e = mp_div_2d(&S2, 2, &S2, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_div_d(&S2, 81, &S2, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_div_d(&S2, 35, &S2, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
#else
   if ((e = mp_div_d(&S2, (mp_digit)11340, &S2, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
#endif

   /** \\S4 = S4 + (S6 * 720) */
   /** tmp = S6 * 720 */
#ifdef MP_8BIT
   if ((e = mp_mul_d(&S6, 45, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul_2d(&tmp, 4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
#else
   if ((e = mp_mul_d(&S6, (mp_digit)720, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
#endif
   /** S4 = S4 + tmp */
   if ((e = mp_add(&S4, &tmp, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 / (-2160) */
   sign = S4.sign;
#ifdef MP_8BIT
   /*
      if ((e = mp_set_long(&tmp, 2160uL)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((e = mp_neg(&tmp, &tmp)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((e = mp_div(&S4, &tmp, &S4, NULL)) != MP_OKAY) {
         goto LTM_ERR;
      }
   */
   /* 2160 = 2^4 * 3^3 * 5 = 2^4 * 27 * 5 <del>= 2^4 * 135</del> */
   if ((e = mp_div_2d(&S4, 4, &S4, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_div_d(&S4, 27, &S4, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_div_d(&S4, 5, &S4, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
#else
   if ((e = mp_div_d(&S4, (mp_digit)2160, &S4, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
#endif
   S4.sign = (sign == MP_NEG)?MP_ZPOS: MP_NEG;
   /** S6 = S6 - S4 */
   if ((e = mp_sub(&S6, &S4, &S6)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S8 = S8 - S2 */
   if ((e = mp_sub(&S8, &S2, &S8)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** P = S1*x^8 + S2*x^7 + S3*x^6 + S4*x^5 + S5*x^4 + S6*x^3 + S7*x^2 + S8*x + S9 */
   if ((e = mp_copy(&S9, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S8, B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S8, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S7, 2 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S7, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S6, 3 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S6, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S5, 4 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S5, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S4, 5 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S3, 6 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S2, 7 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_lshd(&S1, 8 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S1, c)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** P - A*B \\ == zero */

LTM_ERR:
   mp_clear(&b4);
LTM_ERRb4:
   mp_clear(&b3);
LTM_ERRb3:
   mp_clear(&b2);
LTM_ERRb2:
   mp_clear(&b1);
LTM_ERRb1:
   mp_clear(&b0);
LTM_ERRb0:
   mp_clear(&a4);
LTM_ERRa4:
   mp_clear(&a3);
LTM_ERRa3:
   mp_clear(&a2);
LTM_ERRa2:
   mp_clear(&a1);
LTM_ERRa1:
   mp_clear(&a0);
LTM_ERRa0:
   mp_clear_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &S8, &S9, &tmp, &tmp2,
                  &a0, &a1, &a2, &a3, &a4, &b0, &b1, &b2, &b3, &b4,  NULL);
   return e;
}

#endif
