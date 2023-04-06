#include "tommath_private.h"
#ifdef S_MP_SIEVE_SETALL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */




#ifdef MP_USE_MEMOPS
#  include <string.h>
#endif
void s_mp_sieve_setall(mp_single_sieve *bst)
{
   /*
      TODO: To set all even bits to zero
            u32: 5 * 17*257 * 65537
            u64: u32 * 4294967297

            Useful or problems with endianess?
    */
#ifdef MP_USE_MEMOPS
   memset((bst)->content, 0xff, bst->alloc);
#else
   mp_sieve_prime i, bs_size;
   bs_size = bst->alloc / sizeof(mp_sieve_prime);
   for (i = 0; i < bs_size; i++) {
      (bst)->content[i] = MP_SIEVE_FILL;
   }
#endif
}


#endif
