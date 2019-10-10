#include "tommath_private.h"
#ifdef BN_MP_RADIX_SIZE_OVERESTIMATE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/*
   Table of {0, log_2([1..64])} times 2^p where p is the scale
   factor defined in LTM_RADIX_SIZE_SCALE.
 */
/* *INDENT-OFF* */
#define LTM_RADIX_SIZE_SCALE 13
static const uint16_t logbases[65] = {
       0u,     0u,  8192u, 12984u, 16384u,
   19021u, 21176u, 22997u, 24576u, 25968u,
   27213u, 28339u, 29368u, 30314u, 31189u,
   32005u, 32768u, 33484u, 34160u, 34799u,
   35405u, 35981u, 36531u, 37057u, 37560u,
   38042u, 38506u, 38952u, 39381u, 39796u,
   40197u, 40584u, 40960u, 41323u, 41676u,
   42019u, 42352u, 42675u, 42991u, 43298u,
   43597u, 43889u, 44173u, 44451u, 44723u,
   44989u, 45249u, 45503u, 45752u, 45995u,
   46234u, 46468u, 46698u, 46923u, 47144u,
   47360u, 47573u, 47783u, 47988u, 48190u,
   48389u, 48584u, 48776u, 48965u, 49152u
};
/* *INDENT-ON* */
mp_err mp_radix_size_overestimate(const mp_int *a, const int radix, int *size)
{
   mp_int bi_bit_count, bi_k;
   int bit_count;
   mp_err err = MP_OKAY;

   *size = 0;

   if ((radix < 2) || (radix > 64)) {
      return MP_VAL;
   }

   bit_count = mp_count_bits(a) + 1;

   if (bit_count == 0) {
      *size = 2;
      return MP_OKAY;
   }

   if ((err = mp_init_multi(&bi_bit_count, &bi_k, NULL)) != MP_OKAY) {
      return err;
   }

   mp_set_l(&bi_bit_count, bit_count);
   mp_set_u32(&bi_k, logbases[radix]);
   if ((err = mp_mul_2d(&bi_bit_count, LTM_RADIX_SIZE_SCALE, &bi_bit_count)) != MP_OKAY)  goto LTM_ERR;
   if ((err = mp_div(&bi_bit_count, &bi_k, &bi_bit_count, NULL)) != MP_OKAY)              goto LTM_ERR;

   *size = (int)mp_get_l(&bi_bit_count) + 4;
#if ( (defined MP_8BIT) && (INT_MAX > 0xFFFF))
   *size += 3;
#endif

LTM_ERR:
   mp_clear_multi(&bi_bit_count, &bi_k, NULL);
   return err;
}

#endif
