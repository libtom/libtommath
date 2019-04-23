#include "tommath_private.h"
#ifdef BN_S_MP_TOOM_MUL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* multiplication using the Toom-Cook 3-way algorithm
 *
 * Much more complicated than Karatsuba but has a lower
 * asymptotic running time of O(N**1.464).  This algorithm is
 * only particularly useful on VERY large inputs
 * (we're talking 1000s of digits here...).
*/
/*
   Setup from

     Chung, Jaewook, and M. Anwar Hasan. "Asymmetric squaring formulae."
     18th IEEE Symposium on Computer Arithmetic (ARITH'07). IEEE, 2007.

   The interpolation from above needed one temporary variable more
   than the interpolation here:

     Bodrato, Marco, and Alberto Zanoni. "What about Toom-Cook matrices optimality."
     Centro “Vito Volterra” Università di Roma Tor Vergata (2006).
*/
int s_mp_toom_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int S0, S1, S2, S3, S4, tmp, a0, a1, a2, b0, b1, b2;
   int e, B, count;

   /* init temps */
   if ((e = mp_init_multi(&S0, &S1, &S2, &S3, &S4, &tmp, NULL)) != MP_OKAY) {
      return e;
   }

   /* B */
   B = MP_MIN(a->used, b->used) / 3;

   /** a = a2 * x^2 + a1 * x + a0 */
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
   if ((e = mp_init_size(&a2, B + (a->used - (3 * B)))) != MP_OKAY) {
      goto LTM_ERRa2;
   }
   for (; count < a->used; count++) {
         a2.dp[count - 2 * B] = a->dp[count];
         a2.used++;
   }
   mp_clamp(&a2);

   /** b = b2 * x^2 + b1 * x + b0 */
   if ((e = mp_init_size(&b0, B)) != MP_OKAY) {
      goto LTM_ERRb0;
   }
   for (count = 0; count < B; count++) {
         b0.dp[count] = b->dp[count];
         b0.used++;
   }
   mp_clamp(&b0);
   if ((e = mp_init_size(&b1, B)) != MP_OKAY) {
      goto LTM_ERRb1;
   }
   for (; count < (2 * B); count++) {
         b1.dp[count - B] = b->dp[count];
         b1.used++;
   }
   mp_clamp(&b1);
   if ((e = mp_init_size(&b2, B + (b->used - (3 * B)))) != MP_OKAY) {
      goto LTM_ERRb2;
   }
   for (; count < b->used; count++) {
         b2.dp[count - 2 * B] = b->dp[count];
         b2.used++;
   }
   mp_clamp(&b2);


   /** S0 = a0 * b0 */
   if ((e = mp_mul(&a0, &b0, &S0)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /**\\ S1 = (a2+a1+a0) * (b2+b1+b0) **/
   /** S2 = a2 + a1 */
   if ((e = mp_add(&a2, &a1, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 + a0 */
   if ((e = mp_add(&S2, &a0, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S1 = b2 + b1 */
   if ((e = mp_add(&b2, &b1, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S1 = S1 + b0 */
   if ((e = mp_add(&S1, &b0, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S1 = S1 * S2 */
   if ((e = mp_mul(&S1, &S2, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S2 = (4*a2+2*a1+a0) * (4*b2+2*b1+b0) */
   /** S2 = a2 << 2 */
   if ((e = mp_mul_2d(&a2, 2, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = a1 << 1 */
   if ((e = mp_mul_2(&a1, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 + S3 */
   if ((e = mp_add(&S2, &S3, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 + a0 */
   if ((e = mp_add(&S2, &a0, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = b2 << 2 */
   if ((e = mp_mul_2d(&b2, 2, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = b1 << 1 */
   if ((e = mp_mul_2(&b1, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + S4 */
   if ((e = mp_add(&S3, &S4, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + b0 */
   if ((e = mp_add(&S3, &b0, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 * S3 */
   if ((e = mp_mul(&S2, &S3, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** \\S3 = (a2-a1+a0) * (b2-b1+b0) */
   /** S3 = a2 - a1 */
   if ((e = mp_sub(&a2, &a1, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 + a0 */
   if ((e = mp_add(&S3, &a0, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = b2 - b1 */
   if ((e = mp_sub(&b2, &b1, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = S4 + b0 */
   if ((e = mp_add(&S4, &b0, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 * S4 */
   if ((e = mp_mul(&S3, &S4, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S4 = a2 * b2 */
   if ((e = mp_mul(&a2, &b2, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** \\S2 = (S2 - S3)/3 */
   /** S2 = S2 - S3 */
   if ((e = mp_sub(&S2, &S3, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 / 3 \\ this is an exact division */
   if ((e = mp_div_3(&S2, &S2, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S3 = S1 - S3 */
   if ((e = mp_sub(&S1, &S3, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 >> 1 */
   if ((e = mp_div_2(&S3, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S1 = S1 - S0 */
   if ((e = mp_sub(&S1, &S0, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 - S1 */
   if ((e = mp_sub(&S2, &S1, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 >> 1 */
   if ((e = mp_div_2(&S2, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S1 = S1 - S3 */
   if ((e = mp_sub(&S1, &S3, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S1 = S1 - S4 */
   if ((e = mp_sub(&S1, &S4, &S1)) != MP_OKAY){
      goto LTM_ERR;
   }

   /** tmp = S4 << 1 */
   if ((e = mp_mul_2(&S4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 - tmp */
   if ((e = mp_sub(&S2, &tmp, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S3 = S3 - S2 */
   if ((e = mp_sub(&S3, &S2, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** P = S4*x^4+ S2*x^3+ S1*x^2+ S3*x + S0 */
   mp_zero(&tmp);
   if ((e = mp_lshd(&S4, 4 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S2, 3 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S1, 2 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S1, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S3, 1 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S0, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** a * b - P */
   mp_exch(&tmp, c);

LTM_ERR:
   mp_clear(&b2);
LTM_ERRb2:
   mp_clear(&b1);
LTM_ERRb1:
   mp_clear(&b0);
LTM_ERRb0:
   mp_clear(&a2);
LTM_ERRa2:
   mp_clear(&a1);
LTM_ERRa1:
   mp_clear(&a0);
LTM_ERRa0:
   mp_clear_multi(&S0, &S1, &S2, &S3, &S4, &tmp, NULL);
   return e;
}
#endif
