#include "tommath_private.h"
#ifdef MP_SIEVE_CLEAR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Free memory of one sieve */
static void s_clear_one(mp_single_sieve *sieve)
{
   if (sieve->content != NULL) {
      MP_FREE(sieve->content, (size_t)segment->alloc);
   }
}

void mp_sieve_clear(mp_sieve *sieve)
{
   s_clear_one(&(sieve->base));
   s_clear_one(&(sieve->segment));
}


#endif
