#include "tommath_private.h"
#ifdef MP_HASH_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#if defined(MP_16BIT)
#define FNV_1A_INIT ((uint32_t)0x811c9dc5)
#define FNV_1A_PRIME ((uint32_t)0x01000193)
#else
#define FNV_1A_INIT ((uint64_t)0xcbf29ce484222325ULL)
#define FNV_1A_PRIME ((uint64_t)0x100000001b3ULL)
#endif

/* computes hash of mp_int. */
mp_err mp_hash(mp_int *a, mp_hval *hash)
{
   int  x;
   mp_hval hval = FNV_1A_INIT;
   mp_digit r, mask, shift;

   /* FNV-1a algorithm */
   mask = ((mp_digit)1 << 8) - 1uL;
   shift = (mp_digit)(MP_DIGIT_BIT - 8);
   r = 0;
   for (x = a->used; x --> 0;) {
      mp_digit rr = a->dp[x] & mask;
      hval ^= (mp_hval)(a->dp[x] >> 8) | (r << shift);
      hval *= FNV_1A_PRIME;
      r = rr;
   }
   hval ^= mp_isneg(a) ? (mp_hval)1 : (mp_hval)0;
   *hash = hval * FNV_1A_PRIME;

   return MP_OKAY;
}
#endif
