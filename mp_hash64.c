#include "tommath_private.h"
#ifdef MP_HASH64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#define FNV_64_INIT ((uint64_t)0xcbf29ce484222325ULL)
#define FNV_64_PRIME ((uint64_t)0x100000001b3ULL)

/* computes 64 bit hash of mp_int using FNV-1a algorithm. */
mp_err mp_hash64(mp_int *a, uint64_t *hash)
{
   size_t  x, count;
   mp_err  err;
   mp_int  t;
   uint8_t *buf = NULL;
   uint64_t hval = FNV_64_INIT;

   count = mp_ubin_size(a);

   buf = (uint8_t *) MP_MALLOC(count);
   if (buf == NULL) {
      return MP_MEM;
   }

   if ((err = mp_init_copy(&t, a)) != MP_OKAY) {
      return err;
   }

   for (x = count; x --> 0u;) {
      buf[x] = (uint8_t)(t.dp[0] & 255u);
      if ((err = mp_div_2d(&t, 8, &t, NULL)) != MP_OKAY) {
         goto LBL_ERR;
      }
   }

   /* 64 bit FNV-1a */
   for (x = 0; x < count; ++x) {
      hval ^= (uint64_t)buf[x];
      hval *= FNV_64_PRIME;
   }

   *hash = hval;

LBL_ERR:
   MP_FREE_BUF(buf, count);
   mp_clear(&t);
   return err;
}
#endif
