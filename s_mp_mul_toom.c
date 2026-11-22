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
   Bodrato, Marco, and Alberto Zanoni. "What about Toom-Cook matrices optimality."
   Centro Vito Volterra Universita di Roma Tor Vergata (2006)
*/
mp_err s_mp_mul_toom(const mp_int *a, const mp_int *b, mp_int *c)
{

   mp_int S0, S1, S2, S3, S4, T0, a0, a1, a2, b0, b1, b2;
   int B;
   mp_err err;

   /* init temps */
   if ((err = mp_init_multi(&S0, &S1, &S2, &S3, &S4, &T0, NULL)) != MP_OKAY) {
      return err;
   }

   /* B */
   B = MP_MIN(a->used, b->used) / 3;

   /** a = a2 * x^2 + a1 * x + a0; */
   if ((err = s_mp_ro_digs(a,     0, B, &a0)) != MP_OKAY)                                                 goto LBL_ERR;
   if ((err = s_mp_ro_digs(a,     B, B, &a1)) != MP_OKAY)                                                 goto LBL_ERR;
   if ((err = s_mp_ro_digs(a, 2 * B, a->used - 2 * B, &a2)) != MP_OKAY)                                   goto LBL_ERR;
   /** b = b2 * x^2 + b1 * x + b0; */
   if ((err = s_mp_ro_digs(b,     0, B, &b0)) != MP_OKAY)                                                 goto LBL_ERR;
   if ((err = s_mp_ro_digs(b,     B, B, &b1)) != MP_OKAY)                                                 goto LBL_ERR;
   if ((err = s_mp_ro_digs(b, 2 * B, b->used - 2 * B, &b2)) != MP_OKAY)                                   goto LBL_ERR;

   /** \\ S1 = (a2+a1+a0) * (b2+b1+b0); */
   /** S1 = a2 + a1;  */
   if ((err = mp_add(&a2, &a1, &S1)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S1 = S1 + a0;  */
   if ((err = mp_add(&S1, &a0, &S1)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S0 = b2 + b1;  */
   if ((err = mp_add(&b2, &b1, &S0)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S0 = S0 + b0;  */
   if ((err = mp_add(&S0, &b0, &S0)) != MP_OKAY)                                                          goto LBL_ERR;

   /** \\S2 = (4*a2+2*a1+a0) * (4*b2+2*b1+b0); */
   /** S2 = a2 << 2; */
   if ((err = mp_mul_2d(&a2, 2, &S2)) != MP_OKAY)                                                         goto LBL_ERR;
   /** S2 = S2 + a0; */
   if ((err = mp_add(&S2, &a0, &S2)) != MP_OKAY)                                                          goto LBL_ERR;
   /** T0 = a1 << 1; */
   if ((err = mp_mul_2(&a1, &T0)) != MP_OKAY)                                                             goto LBL_ERR;
   /** S2 = S2 + T0; */
   if ((err = mp_add(&S2, &T0, &S2)) != MP_OKAY)                                                          goto LBL_ERR;

   /** T0 = b2 << 2; */
   if ((err = mp_mul_2d(&b2, 2, &T0)) != MP_OKAY)                                                         goto LBL_ERR;
   /** S4 = T0 + b0; */
   if ((err = mp_add(&T0, &b0, &S4)) != MP_OKAY)                                                          goto LBL_ERR;
   /** T0 = b1 << 1;  */
   if ((err = mp_mul_2(&b1, &T0)) != MP_OKAY)                                                             goto LBL_ERR;
   /** S4 = S4 + T0;  */
   if ((err = mp_add(&S4, &T0, &S4)) != MP_OKAY)                                                          goto LBL_ERR;

   /** \\S3 = (a2-a1+a0) * (b2-b1+b0); */
   /** S3 = a2 - a1; */
   if ((err = mp_sub(&a2, &a1, &S3)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S3 = S3 + a0;  */
   if ((err = mp_add(&S3, &a0, &S3)) != MP_OKAY)                                                          goto LBL_ERR;
   /** T0 = b2 - b1;  */
   if ((err = mp_sub(&b2, &b1, &T0)) != MP_OKAY)                                                          goto LBL_ERR;
   /** T0 = T0 + b0;  */
   if ((err = mp_add(&T0, &b0, &T0)) != MP_OKAY)                                                          goto LBL_ERR;

   /* All multiplications together for multithreading */
   /** S1 = S1 * S0;  */
   if ((err = mp_mul(&S1, &S0, &S1)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S2 = S2 * S4;  */
   if ((err = mp_mul(&S2, &S4, &S2)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S3 = S3 * T0;  */
   if ((err = mp_mul(&S3, &T0, &S3)) != MP_OKAY)                                                          goto LBL_ERR;

   /** S0 = a0 * b0; */
   if ((err = mp_mul(&a0, &b0, &S0)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S4 = a2 * b2;   */
   if ((err = mp_mul(&a2, &b2, &S4)) != MP_OKAY)                                                          goto LBL_ERR;

   /** \\S2 = (S2 - S3)/3; */
   /** S2 = S2 - S3; */
   if ((err = mp_sub(&S2, &S3, &S2)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S2 = S2 / 3;  */
   if ((err = s_mp_div_3(&S2, &S2, NULL)) != MP_OKAY)                                                     goto LBL_ERR;

   /** \\ S3 = (S1 - S3)/2; */
   /** S3 = S1 - S3;  */
   if ((err = mp_sub(&S1, &S3, &S3)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S3 = S3 >> 1;  */
   if ((err = mp_div_2(&S3, &S3)) != MP_OKAY)                                                             goto LBL_ERR;
   /** S1 = S1 - S0; */
   if ((err = mp_sub(&S1, &S0, &S1)) != MP_OKAY)                                                          goto LBL_ERR;
   /** \\ S2 = (S2 - S1)/2; */
   /** S2 = S2 - S1;  */
   if ((err = mp_sub(&S2, &S1, &S2)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S2 = S2 >> 1;  */
   if ((err = mp_div_2(&S2, &S2)) != MP_OKAY)                                                             goto LBL_ERR;

   /** \\ S1 = S1 - S3 - S4; */
   /** S1 = S1 - S3;  */
   if ((err = mp_sub(&S1, &S3, &S1)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S1 = S1 - S4;  */
   if ((err = mp_sub(&S1, &S4, &S1)) != MP_OKAY)                                                          goto LBL_ERR;
   /** \\ S2 = S2 - S4*2; */
   /** T0 = S4 << 1;  */
   if ((err = mp_mul_2(&S4, &T0)) != MP_OKAY)                                                             goto LBL_ERR;
   /** S2 = S2 - T0;  */
   if ((err = mp_sub(&S2, &T0, &S2)) != MP_OKAY)                                                          goto LBL_ERR;
   /** S3 = S3 - S2; */
   if ((err = mp_sub(&S3, &S2, &S3)) != MP_OKAY)                                                          goto LBL_ERR;

   /** P = S4*x^4 + S2*x^3 + S1*x^2 + S3*x + S0; */
   if ((err = mp_lshd(&S3, 1 * B)) != MP_OKAY)                                                            goto LBL_ERR;
   if ((err = mp_add(&S3, &S0, &S0)) != MP_OKAY)                                                          goto LBL_ERR;
   if ((err = mp_lshd(&S1, 2 * B)) != MP_OKAY)                                                            goto LBL_ERR;
   if ((err = mp_add(&S1, &S0, &S0)) != MP_OKAY)                                                          goto LBL_ERR;
   if ((err = mp_lshd(&S2, 3 * B)) != MP_OKAY)                                                            goto LBL_ERR;
   if ((err = mp_add(&S2, &S0, &S0)) != MP_OKAY)                                                          goto LBL_ERR;
   if ((err = mp_lshd(&S4, 4 * B)) != MP_OKAY)                                                            goto LBL_ERR;
   if ((err = mp_add(&S4, &S0, &S0)) != MP_OKAY)                                                          goto LBL_ERR;

   /** P - a*b */
   mp_exch(&S0, c);
LBL_ERR:
   mp_clear_multi(&S0, &S1, &S2, &S3, &S4, &T0, NULL);
   return err;
}
#endif
