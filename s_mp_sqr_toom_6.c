#include "tommath_private.h"
#ifdef S_MP_SQR_TOOM_6_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



mp_err s_mp_sqr_toom_6(const mp_int *a, mp_int *c)
{
   mp_int S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, t;
   mp_int a0, a1, a2, a3, a4, a5;
   mp_err err = MP_OKAY;
   int B;

   B = a->used / 6;

   if ((err = mp_init_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &S8, &S9, &S10, &S11, &t, NULL)) != MP_OKAY) {
      return err;
   }

   /** A = a0 + a1 * x + a2 * x^2 + a3 * x^3 + a4 * x^4 + a5*x^5; */
   if ((err = s_mp_ro_digs(a,     0, B, &a0)) != MP_OKAY)                                                 goto LTM_ERR;
   if ((err = s_mp_ro_digs(a,     B, B, &a1)) != MP_OKAY)                                                 goto LTM_ERR;
   if ((err = s_mp_ro_digs(a, 2 * B, B, &a2)) != MP_OKAY)                                                 goto LTM_ERR;
   if ((err = s_mp_ro_digs(a, 3 * B, B, &a3)) != MP_OKAY)                                                 goto LTM_ERR;
   if ((err = s_mp_ro_digs(a, 4 * B, B, &a4)) != MP_OKAY)                                                 goto LTM_ERR;
   if ((err = s_mp_ro_digs(a, 5 * B, a->used - 5 * B, &a5)) != MP_OKAY)                                   goto LTM_ERR;

   /* Evaluation */

   /** S1 = a0 + a2;  */
   if ((err = mp_add(&a0, &a2, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S1 = S1 + a4;  */
   if ((err = mp_add(&S1, &a4, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S11 = a1 + a3;  */
   if ((err = mp_add(&a1, &a3, &S11)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S11 = S11 + a5;  */
   if ((err = mp_add(&S11, &a5, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S4 = S1 - S11;  */
   if ((err = mp_sub(&S1, &S11, &S4)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S5 = S1 + S11;  */
   if ((err = mp_add(&S1, &S11, &S5)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S4 = S4^2;  */
   if ((err = mp_sqr(&S4,&S4)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S5 = S5^2;  */
   if ((err = mp_sqr(&S5,&S5)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S1 = a4 << 4;  */
   if ((err = mp_mul_2d(&a4, 4, &S1)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S1 = S1 + a2;  */
   if ((err = mp_add(&S1, &a2, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S1 = S1 << 4;  */
   if ((err = mp_mul_2d(&S1, 4, &S1)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S1 = S1 + a0;  */
   if ((err = mp_add(&S1, &a0, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S11 = a5 << 4;  */
   if ((err = mp_mul_2d(&a5, 4, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S11 = S11 + a3;  */
   if ((err = mp_add(&S11, &a3, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S11 = S11 << 4;  */
   if ((err = mp_mul_2d(&S11, 4, &S11)) != MP_OKAY)                                                       goto LTM_ERR;
   /** S11 = S11 + a1;  */
   if ((err = mp_add(&S11, &a1, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S11 = S11 << 2;  */
   if ((err = mp_mul_2d(&S11, 2, &S11)) != MP_OKAY)                                                       goto LTM_ERR;
   /** S9 = S1 - S11;  */
   if ((err = mp_sub(&S1, &S11, &S9)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S10 = S1 + S11;  */
   if ((err = mp_add(&S1, &S11, &S10)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S9 = S9^2;  */
   if ((err = mp_sqr(&S9,&S9)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S10 = S10^2;  */
   if ((err = mp_sqr(&S10,&S10)) != MP_OKAY)                                                              goto LTM_ERR;
   /** S1 = a4 << 2;  */
   if ((err = mp_mul_2d(&a4, 2, &S1)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S1 = S1 + a2;  */
   if ((err = mp_add(&S1, &a2, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S1 = S1 << 2;  */
   if ((err = mp_mul_2d(&S1, 2, &S1)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S1 = S1 + a0;  */
   if ((err = mp_add(&S1, &a0, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S11 = a5 << 2;  */
   if ((err = mp_mul_2d(&a5, 2, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S11 = S11 + a3;  */
   if ((err = mp_add(&S11, &a3, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S11 = S11 << 2;  */
   if ((err = mp_mul_2d(&S11, 2, &S11)) != MP_OKAY)                                                       goto LTM_ERR;
   /** S11 = S11 + a1;  */
   if ((err = mp_add(&S11, &a1, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S11 = S11 << 1;  */
   if ((err = mp_mul_2d(&S11, 1, &S11)) != MP_OKAY)                                                       goto LTM_ERR;
   /** S7 = S1 - S11;  */
   if ((err = mp_sub(&S1, &S11, &S7)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S8 = S1 + S11;  */
   if ((err = mp_add(&S1, &S11, &S8)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S7 = S7^2;  */
   if ((err = mp_sqr(&S7,&S7)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S8 = S8^2;  */
   if ((err = mp_sqr(&S8,&S8)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S1 = a0 << 2;  */
   if ((err = mp_mul_2d(&a0, 2, &S1)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S1 = S1 + a2;  */
   if ((err = mp_add(&S1, &a2, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S1 = S1 << 2;  */
   if ((err = mp_mul_2d(&S1, 2, &S1)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S1 = S1 + a4;  */
   if ((err = mp_add(&S1, &a4, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S1 = S1 << 1;  */
   if ((err = mp_mul_2d(&S1, 1, &S1)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S11 = a1 << 2;  */
   if ((err = mp_mul_2d(&a1, 2, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S11 = S11 + a3;  */
   if ((err = mp_add(&S11, &a3, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S11 = S11 << 2;  */
   if ((err = mp_mul_2d(&S11, 2, &S11)) != MP_OKAY)                                                       goto LTM_ERR;
   /** S11 = S11 + a5;  */
   if ((err = mp_add(&S11, &a5, &S11)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S3 = S1 - S11;  */
   if ((err = mp_sub(&S1, &S11, &S3)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S6 = S1 + S11;  */
   if ((err = mp_add(&S1, &S11, &S6)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S3 = S3^2;  */
   if ((err = mp_sqr(&S3,&S3)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S6 = S6^2;  */
   if ((err = mp_sqr(&S6,&S6)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S2 = a0 << 2;  */
   if ((err = mp_mul_2d(&a0, 2, &S2)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S2 = S2 - a1;  */
   if ((err = mp_sub(&S2, &a1, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2 << 2;  */
   if ((err = mp_mul_2d(&S2, 2, &S2)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S2 = S2 + a2;  */
   if ((err = mp_add(&S2, &a2, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2 << 2;  */
   if ((err = mp_mul_2d(&S2, 2, &S2)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S2 = S2 - a3;  */
   if ((err = mp_sub(&S2, &a3, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2 << 2;  */
   if ((err = mp_mul_2d(&S2, 2, &S2)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S2 = S2 + a4;  */
   if ((err = mp_add(&S2, &a4, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2 << 2;  */
   if ((err = mp_mul_2d(&S2, 2, &S2)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S2 = S2 - a5;  */
   if ((err = mp_sub(&S2, &a5, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2^2;  */
   if ((err = mp_sqr(&S2,&S2)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S1 = a0^2;  */
   if ((err = mp_sqr(&a0,&S1)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S11 = a5^2;  */
   if ((err = mp_sqr(&a5, &S11)) != MP_OKAY)                                                              goto LTM_ERR;
   /** S4 = S4 + S5;  */
   if ((err = mp_add(&S4, &S5, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S4 = S4 / 2;  */
   if ((err = mp_div_2d(&S4, 1, &S4, NULL)) != MP_OKAY)                                                   goto LTM_ERR;
   /** S5 = S5 - S4;  */
   if ((err = mp_sub(&S5, &S4, &S5)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2 + S9;  */
   if ((err = mp_add(&S2, &S9, &S2)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S9 = S9 - S10;  */
   if ((err = mp_sub(&S9, &S10, &S9)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S9 = -S9;  */
   if ((err = mp_neg(&S9,&S9)) != MP_OKAY)                                                                goto LTM_ERR;
   /** t = S9 / 2;  */
   if ((err = mp_div_2d(&S9, 1, &t, NULL)) != MP_OKAY)                                                    goto LTM_ERR;
   /** S10 = S10 - t;  */
   if ((err = mp_sub(&S10, &t, &S10)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S10 = S10 - S1;  */
   if ((err = mp_sub(&S10, &S1, &S10)) != MP_OKAY)                                                        goto LTM_ERR;
   /** t = S11 * 1048576;  */
   if ((err = mp_mul_2d(&S11, 20, &t)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S10 = S10 - t;  */
   if ((err = mp_sub(&S10, &t, &S10)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S7 = S7 - S8;  */
   if ((err = mp_sub(&S7, &S8, &S7)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S7 = -S7;  */
   if ((err = mp_neg(&S7,&S7)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S8 = S8 - S1;  */
   if ((err = mp_sub(&S8, &S1, &S8)) != MP_OKAY)                                                          goto LTM_ERR;
   /** t = S11 * 1024;  */
   if ((err = mp_mul_2d(&S11, 10, &t)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S8 = S8 - t;  */
   if ((err = mp_sub(&S8, &t, &S8)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S8 = S8 * 2;  */
   if ((err = mp_mul_2d(&S8, 1, &S8)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S8 = S8 - S7;  */
   if ((err = mp_sub(&S8, &S7, &S8)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S3 = S3 - S6;  */
   if ((err = mp_sub(&S3, &S6, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S3 = -S3;  */
   if ((err = mp_neg(&S3,&S3)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S6 = S6 - S11;  */
   if ((err = mp_sub(&S6, &S11, &S6)) != MP_OKAY)                                                         goto LTM_ERR;
   /** t = S1 * 1024;  */
   if ((err = mp_mul_2d(&S1, 10, &t)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S6 = S6 - t;  */
   if ((err = mp_sub(&S6, &t, &S6)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S6 = S6 * 2;  */
   if ((err = mp_mul_2d(&S6, 1, &S6)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S6 = S6 - S3;  */
   if ((err = mp_sub(&S6, &S3, &S6)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S6 = S6 + S8;  */
   if ((err = mp_add(&S6, &S8, &S6)) != MP_OKAY)                                                          goto LTM_ERR;
   /** t = S4 * 1048577; \\17 * 61681 but 61681 > 2^15 */
#ifdef MP_16BIT
   mp_set_u32(&t, (uint32_t)61681u);
   if ((err = mp_mul_d(&t, 17, &t)) != MP_OKAY)                                                           goto LTM_ERR;
   if ((err = mp_mul(&S4, &t, &t)) != MP_OKAY)                                                            goto LTM_ERR;
#else
   if ((err = mp_mul_d(&S4, 1048577, &t)) != MP_OKAY)                                                     goto LTM_ERR;
#endif
   /** S2 = S2 - t;  */
   if ((err = mp_sub(&S2, &t, &S2)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S2 = -S2;  */
   if ((err = mp_neg(&S2,&S2)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S4 = S4 - S11;  */
   if ((err = mp_sub(&S4, &S11, &S4)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S4 = S4 - S1;  */
   if ((err = mp_sub(&S4, &S1, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** t = S4 * 160;  */
   if ((err = mp_mul_d(&S4, 160, &t)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S6 = S6 - t;  */
   if ((err = mp_sub(&S6, &t, &S6)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S6 = S6 / 360;  */
   if ((err = mp_div_d(&S6, 360, &S6, NULL)) != MP_OKAY)                                                  goto LTM_ERR;
   /** t = S8 * 2;  */
   if ((err = mp_mul_2d(&S8, 1, &t)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S10 = S10 - t;  */
   if ((err = mp_sub(&S10, &t, &S10)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S8 = S8 / 8;  */
   if ((err = mp_div_2d(&S8, 3, &S8, NULL)) != MP_OKAY)                                                   goto LTM_ERR;
   /** S10 = S10 / 64;  */
   if ((err = mp_div_2d(&S10, 6, &S10, NULL)) != MP_OKAY)                                                 goto LTM_ERR;
   /** S8 = S8 - S4;  */
   if ((err = mp_sub(&S8, &S4, &S8)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S10 = S10 - S8;  */
   if ((err = mp_sub(&S10, &S8, &S10)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S8 = S8 * 15;  */
   if ((err = mp_mul_d(&S8, 15, &S8)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S8 = S8 - S10;  */
   if ((err = mp_sub(&S8, &S10, &S8)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S4 = S4 - S6;  */
   if ((err = mp_sub(&S4, &S6, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S8 = S8 / 45;  */
   if ((err = mp_div_d(&S8, 45, &S8, NULL)) != MP_OKAY)                                                   goto LTM_ERR;
   /** S8 = S8 - S4;  */
   if ((err = mp_sub(&S8, &S4, &S8)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S8 = S8 / 3;  */
   if ((err = mp_div_d(&S8, 3, &S8, NULL)) != MP_OKAY)                                                    goto LTM_ERR;
   /** S10 = S10 / 45;  */
   if ((err = mp_div_d(&S10, 45, &S10, NULL)) != MP_OKAY)                                                 goto LTM_ERR;
   /** S10 = S10 - S8;  */
   if ((err = mp_sub(&S10, &S8, &S10)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S10 = S10 / 21;  */
   if ((err = mp_div_d(&S10, 21, &S10, NULL)) != MP_OKAY)                                                 goto LTM_ERR;
   /** S3 = S3 + S7;  */
   if ((err = mp_add(&S3, &S7, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   /** t = S5 * 128;  */
   if ((err = mp_mul_2d(&S5, 7, &t)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S3 = S3 - t;  */
   if ((err = mp_sub(&S3, &t, &S3)) != MP_OKAY)                                                           goto LTM_ERR;
   /** t = S5 * 2048;  */
   if ((err = mp_mul_2d(&S5, 11, &t)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S2 = S2 - t;  */
   if ((err = mp_sub(&S2, &t, &S2)) != MP_OKAY)                                                           goto LTM_ERR;
   /** t = S3 * 100;  */
   if ((err = mp_mul_d(&S3, 100, &t)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S2 = S2 - t;  */
   if ((err = mp_sub(&S2, &t, &S2)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S2 = S2 / 225;  */
   if ((err = mp_div_d(&S2, 225, &S2, NULL)) != MP_OKAY)                                                  goto LTM_ERR;
   /** t = S4 * 4641;  */
   if ((err = mp_mul_d(&S4, 4641, &t)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S2 = S2 - t;  */
   if ((err = mp_sub(&S2, &t, &S2)) != MP_OKAY)                                                           goto LTM_ERR;
   /** t = S6 * 4369;  */
   if ((err = mp_mul_d(&S6, 4369, &t)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S2 = S2 - t;  */
   if ((err = mp_sub(&S2, &t, &S2)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S2 = S2 / 756;  */
   if ((err = mp_div_d(&S2, 756, &S2, NULL)) != MP_OKAY)                                                  goto LTM_ERR;
   /** t = S2 * 900;  */
   if ((err = mp_mul_d(&S2, 900, &t)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S3 = S3 - t;  */
   if ((err = mp_sub(&S3, &t, &S3)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S3 = S3 / 144;  */
   if ((err = mp_div_d(&S3, 144, &S3, NULL)) != MP_OKAY)                                                  goto LTM_ERR;
   /** S5 = S5 - S2;  */
   if ((err = mp_sub(&S5, &S2, &S5)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S5 = S5 - S3;  */
   if ((err = mp_sub(&S5, &S3, &S5)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S9 = S9 / 2;  */
   if ((err = mp_div_2d(&S9, 1, &S9, NULL)) != MP_OKAY)                                                   goto LTM_ERR;
   /** t = S5 * 64;  */
   if ((err = mp_mul_2d(&S5, 6, &t)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S7 = S7 - t;  */
   if ((err = mp_sub(&S7, &t, &S7)) != MP_OKAY)                                                           goto LTM_ERR;
   /** t = t * 16;  */
   if ((err = mp_mul_2d(&t, 4, &t)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S9 = S9 - t;  */
   if ((err = mp_sub(&S9, &t, &S9)) != MP_OKAY)                                                           goto LTM_ERR;
   /** t = S3 * 16;  */
   if ((err = mp_mul_2d(&S3, 4, &t)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S7 = S7 - t;  */
   if ((err = mp_sub(&S7, &t, &S7)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S9 = S9 - S7;  */
   if ((err = mp_sub(&S9, &S7, &S9)) != MP_OKAY)                                                          goto LTM_ERR;
   /** t = S3 * 64;  */
   if ((err = mp_mul_2d(&S3, 6, &t)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S9 = S9 - t;  */
   if ((err = mp_sub(&S9, &t, &S9)) != MP_OKAY)                                                           goto LTM_ERR;
   /** t = S2 * 4;  */
   if ((err = mp_mul_2d(&S2, 2, &t)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S7 = S7 - t;  */
   if ((err = mp_sub(&S7, &t, &S7)) != MP_OKAY)                                                           goto LTM_ERR;
   /** t = S7 * 256;  */
   if ((err = mp_mul_2d(&S7, 8, &t)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S9 = S9 - t;  */
   if ((err = mp_sub(&S9, &t, &S9)) != MP_OKAY)                                                           goto LTM_ERR;
   /** S9 = -S9;  */
   if ((err = mp_neg(&S9,&S9)) != MP_OKAY)                                                                goto LTM_ERR;
   /** S7 = S7 * 189;  */
   if ((err = mp_mul_d(&S7, 189, &S7)) != MP_OKAY)                                                        goto LTM_ERR;
   /** S7 = S7 - S9;  */
   if ((err = mp_sub(&S7, &S9, &S7)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S7 = S7 / 4;  */
   if ((err = mp_div_2d(&S7, 2, &S7, NULL)) != MP_OKAY)                                                   goto LTM_ERR;
   /** S7 = S7 / 5;  */
   if ((err = mp_div_d(&S7, 5, &S7, NULL)) != MP_OKAY)                                                    goto LTM_ERR;
   /** S9 = S9 / 16;  */
   if ((err = mp_div_2d(&S9, 4, &S9, NULL)) != MP_OKAY)                                                   goto LTM_ERR;
   /** S7 = S7 / 9639;  */
   if ((err = mp_div_d(&S7, 9639, &S7, NULL)) != MP_OKAY)                                                 goto LTM_ERR;
   /** S9 = S9 / 2835;  */
   if ((err = mp_div_d(&S9, 2835, &S9, NULL)) != MP_OKAY)                                                 goto LTM_ERR;
   /** S6 = S6 - S10;  */
   if ((err = mp_sub(&S6, &S10, &S6)) != MP_OKAY)                                                         goto LTM_ERR;
   /** S4 = S4 - S8;  */
   if ((err = mp_sub(&S4, &S8, &S4)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S3 = S3 - S9;  */
   if ((err = mp_sub(&S3, &S9, &S3)) != MP_OKAY)                                                          goto LTM_ERR;
   /** S2 = S2 - S7;  */
   if ((err = mp_sub(&S2, &S7, &S2)) != MP_OKAY)                                                          goto LTM_ERR;

   /** t = S3;S3 = S6;S6 = t; */
   mp_exch(&S3, &S6);
   /** t = S4;S4 = S6;S6 = t; */
   mp_exch(&S4, &S6);
   /** t = S5;S5 = S6;S6 = t; */
   mp_exch(&S5, &S6);
   /** t = S7;S7 = S10;S10 = t; */
   mp_exch(&S7, &S10);
   /** t = S8;S8 = S9;S9 = t; */
   mp_exch(&S8, &S9);
   /** t = S7;S7 = S9;S9 = t; */
   mp_exch(&S7, &S9);


   /* Recombination */

   /** P  = S1 + S2 * x + S3 * x^2 + S4 * x^3 + S5 * x^4 + S6 * x^5; */
   /** P += S7 * x^6 + S8 * x^7 + S9 * x^8 + S10 * x^9 + S11 * x^10; */

   if ((err = mp_lshd(&S2, 1 *  B)) != MP_OKAY)                                                           goto LTM_ERR;
   if ((err = mp_add(&S1, &S2, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S3, 2 *  B)) != MP_OKAY)                                                           goto LTM_ERR;
   if ((err = mp_add(&S1, &S3, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S4, 3 *  B)) != MP_OKAY)                                                           goto LTM_ERR;
   if ((err = mp_add(&S1, &S4, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S5, 4 *  B)) != MP_OKAY)                                                           goto LTM_ERR;
   if ((err = mp_add(&S1, &S5, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S6, 5 *  B)) != MP_OKAY)                                                           goto LTM_ERR;
   if ((err = mp_add(&S1, &S6, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S7, 6 *  B)) != MP_OKAY)                                                           goto LTM_ERR;
   if ((err = mp_add(&S1, &S7, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S8, 7 *  B)) != MP_OKAY)                                                           goto LTM_ERR;
   if ((err = mp_add(&S1, &S8, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S9, 8 *  B)) != MP_OKAY)                                                           goto LTM_ERR;
   if ((err = mp_add(&S1, &S9, &S1)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_lshd(&S10, 9 *  B)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_add(&S1, &S10, &S1)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_lshd(&S11, 10 *  B)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_add(&S1, &S11, &S1)) != MP_OKAY)                                                         goto LTM_ERR;

   mp_exch(&S1, c);

   /** P - A^2 */

LTM_ERR:
   mp_clear_multi(&S1, &S2, &S3, &S4, &S5, &S6, &S7, &S8, &S9, &S10, &S11, &t, NULL);
   return err;
}




#endif

