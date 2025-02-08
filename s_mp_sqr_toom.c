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
   mp_int S0, a0, a1, a2;
   int B;
   mp_err err = MP_OKAY;

   /* init temps */
   if ((err = mp_init(&S0)) != MP_OKAY)                                  MP_TRACE_ERROR(err, LTM_ERR);

   /* B */
   B = a->used / 3;

   /** a = a2 * x^2 + a1 * x + a0; */
   if ((err = mp_init_size(&a0, B)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_a0);
   if ((err = mp_init_size(&a1, B)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_a1);
   if ((err = mp_init_size(&a2, a->used - (2 * B))) != MP_OKAY)          MP_TRACE_ERROR(err, LTM_ERR_a2);

   a0.used = a1.used = B;
   a2.used = a->used - 2 * B;
   s_mp_copy_digs(a0.dp, a->dp, a0.used);
   s_mp_copy_digs(a1.dp, a->dp + B, a1.used);
   s_mp_copy_digs(a2.dp, a->dp + 2 * B, a2.used);
   mp_clamp(&a0);
   mp_clamp(&a1);
   mp_clamp(&a2);

   /** S0 = a0^2;  */
   if ((err = mp_sqr(&a0, &S0)) != MP_OKAY)                              MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** \\S1 = (a2 + a1 + a0)^2 */
   /** \\S2 = (a2 - a1 + a0)^2  */
   /** \\S1 = a0 + a2; */
   /** a0 = a0 + a2; */
   if ((err = mp_add(&a0, &a2, &a0)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);
   /** \\S2 = S1 - a1; */
   /** b = a0 - a1; */
   if ((err = mp_sub(&a0, &a1, b)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);
   /** \\S1 = S1 + a1; */
   /** a0 = a0 + a1; */
   if ((err = mp_add(&a0, &a1, &a0)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);
   /** \\S1 = S1^2;  */
   /** a0 = a0^2; */
   if ((err = mp_sqr(&a0, &a0)) != MP_OKAY)                              MP_TRACE_ERROR(err, LTM_ERR_ALL);
   /** \\S2 = S2^2;  */
   /** b = b^2; */
   if ((err = mp_sqr(b, b)) != MP_OKAY)                                  MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** \\ S3 = 2 * a1 * a2  */
   /** \\S3 = a1 * a2;  */
   /** a1 = a1 * a2; */
   if ((err = mp_mul(&a1, &a2, &a1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);
   /** \\S3 = S3 << 1;  */
   /** a1 = a1 << 1; */
   if ((err = mp_mul_2(&a1, &a1)) != MP_OKAY)                            MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** \\S4 = a2^2;  */
   /** a2 = a2^2; */
   if ((err = mp_sqr(&a2, &a2)) != MP_OKAY)                              MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** \\ tmp = (S1 + S2)/2  */
   /** \\tmp = S1 + S2; */
   /** b = a0 + b; */
   if ((err = mp_add(&a0, b, b)) != MP_OKAY)                             MP_TRACE_ERROR(err, LTM_ERR_ALL);
   /** \\tmp = tmp >> 1; */
   /** b = b >> 1; */
   if ((err = mp_div_2(b, b)) != MP_OKAY)                                MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** \\ S1 = S1 - tmp - S3  */
   /** \\S1 = S1 - tmp; */
   /** a0 = a0 - b; */
   if ((err = mp_sub(&a0, b, &a0)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);
   /** \\S1 = S1 - S3;  */
   /** a0 = a0 - a1; */
   if ((err = mp_sub(&a0, &a1, &a0)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);

   /** \\S2 = tmp - S4 -S0  */
   /** \\S2 = tmp - S4;  */
   /** b = b - a2; */
   if ((err = mp_sub(b, &a2, b)) != MP_OKAY)                             MP_TRACE_ERROR(err, LTM_ERR_ALL);
   /** \\S2 = S2 - S0;  */
   /** b = b - S0; */
   if ((err = mp_sub(b, &S0, b)) != MP_OKAY)                             MP_TRACE_ERROR(err, LTM_ERR_ALL);


   /** \\P = S4*x^4 + S3*x^3 + S2*x^2 + S1*x + S0; */
   /** P = a2*x^4 + a1*x^3 + b*x^2 + a0*x + S0; */

   if ((err = mp_lshd(&a2, 4 * B)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_lshd(&a1, 3 * B)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_lshd(b, 2 * B)) != MP_OKAY)                             MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_lshd(&a0, 1 * B)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_add(&a2, &a1, &a2)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_add(&a2, b, b)) != MP_OKAY)                             MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_add(b, &a0, b)) != MP_OKAY)                             MP_TRACE_ERROR(err, LTM_ERR_ALL);
   if ((err = mp_add(b, &S0, b)) != MP_OKAY)                             MP_TRACE_ERROR(err, LTM_ERR_ALL);
   /** a^2 - P  */


LTM_ERR_ALL:
   mp_clear(&a2);
LTM_ERR_a2:
   mp_clear(&a1);
LTM_ERR_a1:
   mp_clear(&a0);
LTM_ERR_a0:
   mp_clear(&S0);
LTM_ERR:
   return err;
}

#endif
