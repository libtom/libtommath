#include "tommath_private.h"
#ifdef BN_S_MP_TOOM_SQR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* squaring using Toom-Cook 3-way algorithm */
/*
   Setup and interpolation from

     Chung, Jaewook, and M. Anwar Hasan. "Asymmetric squaring formulae."
     18th IEEE Symposium on Computer Arithmetic (ARITH'07). IEEE, 2007.
*/
int s_mp_toom_sqr(const mp_int *a, mp_int *b)
{
   mp_int S0, S1, S2, S3, S4, tmp, a0, a1, a2;
   int e, B, count;

   /* init temps */
   if ((e = mp_init_multi(&S0, &S1, &S2, &S3, &S4, &tmp, NULL)) != MP_OKAY) {
      return e;
   }

   /* B */
   B = a->used / 3;

   /** a = a2 * B**2 + a1 * B + a0 */
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

   /** S0 = a0^2  */
   if ((e = mp_sqr(&a0, &S0)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** \\S1 = (a2 + a1 + a0)^2 */
   /** S1 = a2 + a1  */
   if ((e = mp_add(&a2, &a1, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S1 = S1 + a0  */
   if ((e = mp_add(&S1, &a0, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S1 = S1^2  */
   if ((e = mp_sqr(&S1, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** \\ S2 = (a2 - a1 + a0)^2  */
   /** S2 = a2 - a1  */
   if ((e = mp_sub(&a2, &a1, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 + a0  */
   if ((e = mp_add(&S2, &a0, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2^2  */
   if ((e = mp_sqr(&S2, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** \\ S3 = 2 * a1 * a2  */
   /** S3 = a1 * a2  */
   if ((e = mp_mul(&a1, &a2, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S3 = S3 << 1  */
   if ((e = mp_mul_2(&S3, &S3)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** S4 = a2^2  */
   if ((e = mp_sqr(&a2, &S4)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** \\ tmp = (S1 + S2)/2  */
   /** tmp = S1 + S2 */
   if ((e = mp_add(&S1, &S2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** tmp = tmp >> 1 */
   if ((e = mp_div_2(&tmp, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** \\ S1 = S1 - tmp - S3  */
   /** S1 = S1 - tmp */
   if ((e = mp_sub(&S1, &tmp, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S1 = S1 - S3  */
   if ((e = mp_sub(&S1, &S3, &S1)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /** \\S2 = tmp - S4 -S0  */
   /** S2 = tmp - S4  */
   if ((e = mp_sub(&tmp, &S4, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** S2 = S2 - S0  */
   if ((e = mp_sub(&S2, &S0, &S2)) != MP_OKAY) {
      goto LTM_ERR;
   }


   /** P = S4*x^4 + S3*x^3 + S2*x^2 + S1*x + S0 */
   mp_zero(&tmp);
   if ((e = mp_lshd(&S4, 4 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S4, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S3, 3 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S3, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S2, 2 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S2, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_lshd(&S1, 1 * B)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S1, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_add(&tmp, &S0, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /** a^2 - P  */
   mp_exch(&tmp, b);

LTM_ERR:
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
