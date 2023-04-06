#include "tommath_private.h"
#ifdef MP_SIEVE_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Set the default values for the sieve */
void mp_sieve_init(mp_sieve *sieve)
{
   sieve->base.content = NULL;
   sieve->segment.content = NULL;
   sieve->single_segment_a = 0uL;
}



#endif
