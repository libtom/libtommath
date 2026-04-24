#include "tommath_private.h"
#ifdef S_MP_FP_LOG_D_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


static void s_mp_32_shr(uint32_t high, uint32_t low, uint32_t shift, uint32_t *r_high, uint32_t *r_low)
{
   if (shift == 0) {
      *r_high = high;
      *r_low = low;
   } else if (shift >= 64) {
      *r_high = 0;
      *r_low = 0;
   } else if (shift >= 32) {
      *r_high = 0;
      *r_low = high >> (shift - 32);
   } else {
      *r_high = high >> shift;
      *r_low = (low >> shift) | (high << (32 - shift));
   }
}

#define MP_FP_16_HALF (1UL << 15)
#define MP_FP_16_ONE (1UL << 16)
#define MP_FP_16_TWO (1UL << 17)

/* Fixed point bitwise logarithm base two of "x" with precision "p"  */
static uint32_t s_mp_32_log2(uint32_t x)
{
   uint32_t result = 0, a, b, bit;

   if (x == 0) {
      /* works because log(x) < x */
      return UINT32_MAX;
   }

   /* log2(1) == 0 */
   if (x == MP_FP_16_ONE) {
      return 0;
   }

   while (x >= MP_FP_16_TWO) {
      result += MP_FP_16_ONE;
      x >>= 1;
   }

   bit = MP_FP_16_HALF;

   while (bit > 0) {
      s_mp_32_umul32(x, x, &a, &b);
      s_mp_32_shr(a, b, 16, &a, &x);

      if (x >= (1 << 17)) {
         result |= bit;
         x >>= 1;
      }
      bit >>= 1;
   }
   return result;
}

/* Approximate the base two logarithm of "a" */
mp_err s_mp_fp_log_d(const mp_int *a, mp_word *c)
{
   mp_err err;
   int la;
   int prec = MP_PRECISION_FIXED_LOG;
   uint32_t tmp, la_word;
   mp_int t;

   la = mp_count_bits(a) - 1;

   /* We don't use the whole number, just the most significant "prec" bits */
   if (la > prec) {
      if ((err = mp_init(&t)) != MP_OKAY)                                                                 goto LTM_ERR;
      /* Get enough msb-bits for the chosen precision */
      if ((err = mp_div_2d(a, la - prec, &t, NULL)) != MP_OKAY)                                           goto LTM_ERR;
      tmp = mp_get_mag_u32(&t);
      /* Compute the low precision approximation for the fractional part */
      la_word = s_mp_32_log2(tmp<<MP_FP_SCALE_LOG);
      if (la_word == UINT32_MAX) {
         err = MP_VAL;
         goto LTM_ERR;
      }
      /* Compute the integer part and add it */
      tmp = (uint32_t)((la - prec)<<MP_FP_SCALE_LOG);
      la_word += tmp;
      mp_clear(&t);
   } else {
      tmp = mp_get_mag_u32(a);
      la_word = s_mp_32_log2(tmp<<MP_FP_SCALE_LOG);
      tmp = (uint32_t)la << prec;
      la_word += tmp;
   }

   *c = (mp_word)la_word;

   return MP_OKAY;
LTM_ERR:
   mp_clear(&t);
   return err;
}

#endif
