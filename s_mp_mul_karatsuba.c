#include "tommath_private.h"
#ifdef S_MP_MUL_KARATSUBA_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* c = |a| * |b| using Karatsuba Multiplication using
 * three half size multiplications
 *
 * Let B represent the radix [e.g. 2**MP_DIGIT_BIT] and
 * let n represent half of the number of digits in
 * the min(a,b)
 *
 * a = a1 * B**n + a0
 * b = b1 * B**n + b0
 *
 * Then, a * b =>
   a1b1 * B**2n + ((a1 + a0)(b1 + b0) - (a0b0 + a1b1)) * B + a0b0
 *
 * Note that a1b1 and a0b0 are used twice and only need to be
 * computed once.  So in total three half size (half # of
 * digit) multiplications are performed, a0b0, a1b1 and
 * (a1+b1)(a0+b0)
 *
 * Note that a multiplication of half the digits requires
 * 1/4th the number of single precision multiplications so in
 * total after one call 25% of the single precision multiplications
 * are saved.  Note also that the call to mp_mul can end up back
 * in this function if the a0, a1, b0, or b1 are above the threshold.
 * This is known as divide-and-conquer and leads to the famous
 * O(N**lg(3)) or O(N**1.584) work which is asymptotically lower than
 * the standard O(N**2) that the baseline/comba methods use.
 * Generally though the overhead of this method doesn't pay off
 * until a certain size (N ~ 80) is reached.
 */
mp_err s_mp_mul_karatsuba(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int  x0, x1, y0, y1, t1, t2, x0y0, x1y1;
   int  B;
   mp_err  err;

   /* min # of digits */
   B = MP_MIN(a->used, b->used);

   /* now divide in two */
   B = B >> 1;

   /* copy all the temps */
   if ((err = s_mp_ro_digs(a, 0, B, &x0)) != MP_OKAY)                                                     goto LBL_ERR;
   if ((err = s_mp_ro_digs(a, B, a->used - B, &x1)) != MP_OKAY)                                           goto LBL_ERR;
   if ((err = s_mp_ro_digs(b, 0, B, &y0)) != MP_OKAY)                                                     goto LBL_ERR;
   if ((err = s_mp_ro_digs(b, B, b->used - B, &y1)) != MP_OKAY)                                           goto LBL_ERR;

   /* init temps */
   if ((err = mp_init_size(&t1, B * 2)) != MP_OKAY) {
      goto LBL_ERR;
   }
   if ((err = mp_init_size(&t2, B * 2)) != MP_OKAY) {
      goto T1;
   }
   if ((err = mp_init_size(&x0y0, B * 2)) != MP_OKAY) {
      goto T2;
   }
   if ((err = mp_init_size(&x1y1, B * 2)) != MP_OKAY) {
      goto X0Y0;
   }

   /* now shift the digits */
   x0.used = y0.used = B;
   x1.used = a->used - B;
   y1.used = b->used - B;

   /* we copy the digits directly instead of using higher level functions
    * since we also need to shift the digits
    */
   s_mp_copy_digs(x0.dp, a->dp, x0.used);
   s_mp_copy_digs(y0.dp, b->dp, y0.used);
   s_mp_copy_digs(x1.dp, a->dp + B, x1.used);
   s_mp_copy_digs(y1.dp, b->dp + B, y1.used);

   /* now calc x1+x0 and y1+y0 */
   /* t1 = x1 - x0 */
   if ((err = s_mp_add(&x1, &x0, &t1)) != MP_OKAY)                                                           goto X1Y1;
   /* t2 = y1 - y0 */
   if ((err = s_mp_add(&y1, &y0, &t2)) != MP_OKAY)                                                           goto X1Y1;
   /* t1 = (x1 + x0) * (y1 + y0) */
   if ((err = mp_mul(&t1, &t2, &t1)) != MP_OKAY)                                                             goto X1Y1;

   /* now calc the products x0y0 and x1y1 */
   /* x0y0 = x0*y0 */
   if ((err = mp_mul(&x0, &y0, &x0y0)) != MP_OKAY)                                                           goto X1Y1;
   /* x1y1 = x1*y1 */
   if ((err = mp_mul(&x1, &y1, &x1y1)) != MP_OKAY)                                                           goto X1Y1;


   /* add x0y0 */
   /* t2 = x0y0 + x1y1 */
   if ((err = mp_add(&x0y0, &x1y1, &t2)) != MP_OKAY)                                                         goto X1Y1;
   /* t1 = (x1+x0)*(y1+y0) - (x1y1 + x0y0) */
   if ((err = s_mp_sub(&t1, &t2, &t1)) != MP_OKAY)                                                           goto X1Y1;

   /* shift by B */
   /* t1 = (x0y0 + x1y1 - (x1-x0)*(y1-y0))<<B */
   if ((err = mp_lshd(&t1, B)) != MP_OKAY)                                                                   goto X1Y1;
   /* x1y1 = x1y1 << 2*B */
   if ((err = mp_lshd(&x1y1, B * 2)) != MP_OKAY)                                                             goto X1Y1;
   /* t1 = x0y0 + t1 */
   if ((err = mp_add(&x0y0, &t1, &t1)) != MP_OKAY)                                                           goto X1Y1;
   /* t1 = x0y0 + t1 + x1y1 */
   if ((err = mp_add(&t1, &x1y1, c)) != MP_OKAY)                                                             goto X1Y1;

X1Y1:
   mp_clear(&x1y1);
X0Y0:
   mp_clear(&x0y0);
T1:
   mp_clear(&t1);
T2:
   mp_clear(&t2);
LBL_ERR:
   return err;
}
#endif
