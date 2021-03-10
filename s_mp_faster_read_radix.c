#include "tommath_private.h"
#ifdef S_MP_FASTER_READ_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* TODO: It is tunable */
#define MP_READ_RADIX_CUTOFF_MULTIPLICATOR 3

/* for(n=2,64,t = ceil(log(2^100)/log(n)); printf(t", "); ) */
static const uint8_t s_read_radix_cutoff[65] = { 0, 0,                            /*  0 .. 1*/
                                                 100, 64, 50, 44, 39, 36, 34, 32, /*  2 .. 9 */
                                                 31, 29, 28, 28, 27, 26, 25, 25,  /* 10 .. 17 */
                                                 24, 24, 24, 23, 23, 23, 22, 22,  /* 18 .. 25 */
                                                 22, 22, 21, 21, 21, 21, 20, 20,  /* 26 .. 33 */
                                                 20, 20, 20, 20, 20, 19, 19, 19,  /* 34 .. 41 */
                                                 19, 19, 19, 19, 19, 19, 18, 18,  /* 42 .. 49 */
                                                 18, 18, 18, 18, 18, 18, 18, 18,  /* 51 .. 57 */
                                                 18, 17, 17, 17, 17, 17, 17       /* 58 .. 64 */
                                               };

/* This is in mp_prime_is_prime.c and can be reused */
static int s_floor_ilog2(int value)
{
   int r = 0;
   while ((value >>= 1) != 0) {
      r++;
   }
   return r;
}

mp_err s_mp_faster_read_radix(mp_int *a, const char *str, size_t start, size_t end, int radix)
{
   size_t len, mid;
   mp_int A, B, m;
   mp_err err = MP_OKAY;

   len = end - start;

   if (len < (size_t)(s_read_radix_cutoff[radix] * MP_READ_RADIX_CUTOFF_MULTIPLICATOR)) {
      return s_mp_slower_read_radix(a, str, start, end, radix);
   }

   mid = len / 2u;

   if ((err = mp_init_set(&m, (mp_digit)radix)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_init_multi(&A, &B, NULL)) != MP_OKAY) {
      mp_clear(&m);
      return err;
   }

   if ((err = s_mp_slower_read_radix(&A, str, start, start + mid + 1, radix)) != MP_OKAY)                goto LTM_ERR;
   if ((err = s_mp_slower_read_radix(&B, str, start + mid +1, end, radix)) != MP_OKAY)                   goto LTM_ERR;

   if (MP_IS_2EXPT((unsigned int)radix)) {
      if ((err = mp_mul_2d(&A, (int)(((len - mid) - 1u) * (size_t)s_floor_ilog2(radix)), &A)) != MP_OKAY)goto LTM_ERR;
   } else {
      if ((err = mp_expt_n(&m, (int)((len - mid) - 1u), &m)) != MP_OKAY)                                 goto LTM_ERR;
      if ((err = mp_mul(&A, &m, &A)) != MP_OKAY)                                                         goto LTM_ERR;
   }
   if ((err = mp_add(&A, &B, a)) != MP_OKAY)                                                             goto LTM_ERR;

LTM_ERR:
   mp_clear_multi(&A, &B, &m, NULL);
   return err;
}

#endif
