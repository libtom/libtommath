#include "tommath_private.h"
#ifdef S_MP_ERATOSTHENES_SEGMENT_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/*
 * Init sieve "segment" of the range [a,b]:
 * allocate memory, set actual size and allocated size and fill it from sieve "base"
 * TODO: merge with s_mp_eratosthenes_init()
 */
mp_err s_mp_eratosthenes_segment_init(mp_sieve_prime a, mp_sieve_prime b,
                                      mp_single_sieve *segment, mp_single_sieve *base)
{
   mp_sieve_prime n;
   size_t bits_in_sp;

   n = b - a;
   bits_in_sp = ((size_t)n  + MP_SIEVE_PRIME_NUM_BITS - 1u)/MP_SIEVE_PRIME_NUM_BITS;
   bits_in_sp = bits_in_sp * sizeof(mp_sieve_prime) + sizeof(mp_sieve_prime);
   /* All segments besides the last one have the same size--we can keep the memory */
   if (segment->content == NULL) {
      segment->content = MP_MALLOC(bits_in_sp);
      if (segment->content == NULL) {
         return MP_MEM;
      }
   }
   segment->alloc = (mp_sieve_prime)bits_in_sp;
   segment->size = n;
   s_mp_eratosthenes_segment(a, b, base, segment);
   return MP_OKAY;
}


#endif
