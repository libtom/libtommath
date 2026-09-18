#include "tommath_private.h"
#ifdef MP_SMALL_PRIME_SIEVE_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



mp_err mp_small_prime_sieve_init(mp_erat_sieve *sieve, bool warmup)
{
   mp_err err = MP_OKAY;
   sieve->base.content = NULL;
   sieve->base.alloc = 0;
   sieve->base.size = 0;
   sieve->segment.content = NULL;
   sieve->segment.alloc = 0;
   sieve->segment.size = 0;
   sieve->single_segment_a = 0;
   if (warmup) {
      err = s_mp_erat_eratosthenes_init(ERAT_UINT_MAX_SQRT, &(sieve->base));
   }
   return err;
}


#endif
