#include "tommath_private.h"
#ifdef S_MP_MUL_TOOM_C
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
   This file contains code from J. Arndt's book  "Matters Computational"
   and the accompanying FXT-library with permission of the author.
*/

/*
   Setup from

     Chung, Jaewook, and M. Anwar Hasan. "Asymmetric squaring formulae."
     18th IEEE Symposium on Computer Arithmetic (ARITH'07). IEEE, 2007.

   The interpolation from above needed one temporary variable more
   than the interpolation here:

     Bodrato, Marco, and Alberto Zanoni. "What about Toom-Cook matrices optimality."
     Centro Vito Volterra Universita di Roma Tor Vergata (2006)
*/

mp_err s_mp_mul_toom(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int S1, S2, T1, a0, a1, a2, b0, b1, b2;
   int B;
   mp_err err = MP_OKAY;

   /* init temps */
   if ((err = mp_init_multi(&S1, &S2, &T1, NULL)) != MP_OKAY)            MP_TRACE_ERROR(err, LTM_ERR);

   /* B */
   B = MP_MIN(a->used, b->used) / 3;

   /** a = a2 * x^2 + a1 * x + a0; */
   if ((err = mp_init_size(&a0, B)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_a0);
   if ((err = mp_init_size(&a1, B)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_a1);
   if ((err = mp_init_size(&a2, a->used - 2 * B)) != MP_OKAY)            MP_TRACE_ERROR(err, LTM_ERR_a2);

   a0.used = a1.used = B;
   a2.used = a->used - 2 * B;
   s_mp_copy_digs(a0.dp, a->dp, a0.used);
   s_mp_copy_digs(a1.dp, a->dp + B, a1.used);
   s_mp_copy_digs(a2.dp, a->dp + 2 * B, a2.used);
   mp_clamp(&a0);
   mp_clamp(&a1);
   mp_clamp(&a2);

   /** b = b2 * x^2 + b1 * x + b0; */
   if ((err = mp_init_size(&b0, B)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_b0);
   if ((err = mp_init_size(&b1, B)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_b1);
   if ((err = mp_init_size(&b2, b->used - 2 * B)) != MP_OKAY)            MP_TRACE_ERROR(err, LTM_ERR_b2);

   b0.used = b1.used = B;
   b2.used = b->used - 2 * B;
   s_mp_copy_digs(b0.dp, b->dp, b0.used);
   s_mp_copy_digs(b1.dp, b->dp + B, b1.used);
   s_mp_copy_digs(b2.dp, b->dp + 2 * B, b2.used);
   mp_clamp(&b0);
   mp_clamp(&b1);
   mp_clamp(&b2);

   /** \\ S1 = (a2+a1+a0) * (b2+b1+b0); */
   /** T1 = a2 + a1; */
   if ((err = mp_add(&a2, &a1, &T1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S2 = T1 + a0; */
   if ((err = mp_add(&T1, &a0, &S2)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** c = b2 + b1; */
   if ((err = mp_add(&b2, &b1, c)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S1 = c + b0; */
   if ((err = mp_add(c, &b0, &S1)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S1 = S1 * S2; */
   if ((err = mp_mul(&S1, &S2, &S1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** \\S2 = (4*a2+2*a1+a0) * (4*b2+2*b1+b0); */
   /** T1 = T1 + a2; */
   if ((err = mp_add(&T1, &a2, &T1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** T1 = T1 << 1; */
   if ((err = mp_mul_2(&T1, &T1)) != MP_OKAY)                            MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** T1 = T1 + a0; */
   if ((err = mp_add(&T1, &a0, &T1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** c = c + b2; */
   if ((err = mp_add(c, &b2, c)) != MP_OKAY)                             MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** c = c << 1; */
   if ((err = mp_mul_2(c, c)) != MP_OKAY)                                MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** c = c + b0; */
   if ((err = mp_add(c, &b0, c)) != MP_OKAY)                             MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S2 = T1 * c; */
   if ((err = mp_mul(&T1, c, &S2)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** \\S3 = (a2-a1+a0) * (b2-b1+b0); */
   /** a1 = a2 - a1; */
   if ((err = mp_sub(&a2, &a1, &a1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** a1 = a1 + a0; */
   if ((err = mp_add(&a1, &a0, &a1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** b1 = b2 - b1; */
   if ((err = mp_sub(&b2, &b1, &b1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** b1 = b1 + b0; */
   if ((err = mp_add(&b1, &b0, &b1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** a1 = a1 * b1; */
   if ((err = mp_mul(&a1, &b1, &a1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** b1 = a2 * b2; */
   if ((err = mp_mul(&a2, &b2, &b1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** \\S2 = (S2 - S3)/3; */
   /** S2 = S2 - a1; */
   if ((err = mp_sub(&S2, &a1, &S2)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S2 = S2 / 3; \\ this is an exact division  */
   if ((err = s_mp_div_3(&S2, &S2, NULL)) != MP_OKAY)                    MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** a1 = S1 - a1; */
   if ((err = mp_sub(&S1, &a1, &a1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** a1 = a1 >> 1; */
   if ((err = mp_div_2(&a1, &a1)) != MP_OKAY)                            MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** a0 = a0 * b0; */
   if ((err = mp_mul(&a0, &b0, &a0)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S1 = S1 - a0; */
   if ((err = mp_sub(&S1, &a0, &S1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S2 = S2 - S1; */
   if ((err = mp_sub(&S2, &S1, &S2)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S2 = S2 >> 1; */
   if ((err = mp_div_2(&S2, &S2)) != MP_OKAY)                            MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S1 = S1 - a1; */
   if ((err = mp_sub(&S1, &a1, &S1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S1 = S1 - b1; */
   if ((err = mp_sub(&S1, &b1, &S1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** T1 = b1 << 1; */
   if ((err = mp_mul_2(&b1, &T1)) != MP_OKAY)                            MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** S2 = S2 - T1; */
   if ((err = mp_sub(&S2, &T1, &S2)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** a1 = a1 - S2; */
   if ((err = mp_sub(&a1, &S2, &a1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);


   /** P = b1*x^4+ S2*x^3+ S1*x^2+ a1*x + a0; */
   if ((err = mp_lshd(&b1, 4 * B)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_lshd(&S2, 3 * B)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_add(&b1, &S2, &b1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_lshd(&S1, 2 * B)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_add(&b1, &S1, &b1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_lshd(&a1, 1 * B)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_add(&b1, &a1, &b1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_add(&b1, &a0, c)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** a * b - P */


LTM_ERR_ALL:
   mp_clear(&b2);
LTM_ERR_b2:
   mp_clear(&b1);
LTM_ERR_b1:
   mp_clear(&b0);
LTM_ERR_b0:
   mp_clear(&a2);
LTM_ERR_a2:
   mp_clear(&a1);
LTM_ERR_a1:
   mp_clear(&a0);
LTM_ERR_a0:
   mp_clear_multi(&S1, &S2, &T1, NULL);
LTM_ERR:
   return err;
}

#endif
