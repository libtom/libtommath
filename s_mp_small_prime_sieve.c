#include "tommath_private.h"
#ifdef S_MP_SMALL_PRIME_SIEVE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifdef MP_USE_MEMOPS
#include <string.h>
#endif

ERAT_UINT s_mp_erat_isqrt(ERAT_UINT n)
{
   ERAT_UINT s, rem, root;

   if (n < 1) {
      return 0;
   }
   /* highest power of four <= n */
   s = ((ERAT_UINT) 1) << (BITS_IN_ERAT_UINT - 2);

   rem = n;
   root = 0;
   while (s > n) {
      s >>= 2;
   }
   while (s != 0) {
      if (rem >= (s | root)) {
         rem -= (s | root);
         root >>= 1;
         root |= s;
      } else {
         root >>= 1;
      }
      s >>= 2;
   }
   return root;
}
void s_mp_mp_erat_sieve_setall(mp_erat_single_sieve *bst)
{
#ifdef MP_USE_MEMOPS
   memset(bst->content, 0xFF, bst->alloc);
#else
   size_t i, bs_size;
   bs_size = bst->alloc / sizeof(ERAT_UINT);
   for (i = 0; i < bs_size; i++) {
      bst->content[i] = ERAT_UINT_MAX;
   }
#endif
}


/* TODO: inline? macros? */

/* Set bit at position n to zero */
void s_mp_mp_mp_erat_sieve_clear_bit(mp_erat_single_sieve *bst, ERAT_UINT n)
{
   ((*((bst)->content+(n/BITS_IN_ERAT_UINT)) &= ~(1lu<<(n % BITS_IN_ERAT_UINT))));
}

/* Read bit at position n */
ERAT_UINT s_mp_mp_erat_sieve_get_bit(mp_erat_single_sieve *bst, ERAT_UINT n)
{
   return (((*((bst)->content+(n/BITS_IN_ERAT_UINT)) & (1lu<<(n % BITS_IN_ERAT_UINT))) != 0));
}

/* Seek set bit after position n */
ERAT_UINT s_mp_mp_erat_sieve_nextset(mp_erat_single_sieve *bst, ERAT_UINT n)
{
   while ((n < (ERAT_UINT)(((bst)->size)) && (!s_mp_mp_erat_sieve_get_bit(bst, n)))) {
      n++;
   }
   return n;
}

/*
 * Initiate a sieve that stores the odd numbers only:
 * allocate memory, set actual size and allocated size and fill it
 */
mp_err s_mp_erat_eratosthenes_init(ERAT_UINT n, mp_erat_single_sieve *bst)
{
   bst->size = n;
   /* The number BITS we need */
   n = (n + 1) / 2;
   /* The number of BYTES we need is an eighth of the above. */
   n = (n + 1) / 8;

   bst->content = (ERAT_UINT *)MP_MALLOC(n + sizeof(ERAT_UINT));
   if (bst->content == NULL) {
      return MP_MEM;
   }
   /* Take note */
   bst->alloc = n + sizeof(ERAT_UINT);
   /* Fill sieve */
   s_mp_erat_eratosthenes(bst);
   return MP_OKAY;
}

/*
 * Build a segment sieve with the largest reasonable size. "a" is the start of
 * the sieve, Size is MIN(range_a_b,ERAT_UINT_MAX-a)
 */
mp_err s_mp_erat_init_single_segment_with_start(
   ERAT_UINT a,
   mp_erat_single_sieve *base_sieve,
   mp_erat_single_sieve *single_segment,
   ERAT_UINT *single_segment_a)
{
   ERAT_UINT b, n;
   mp_err e = MP_OKAY;

   if (a > (ERAT_BIGGEST_PRIME - ERAT_UINT_MAX_SQRT)) {
      b = ERAT_BIGGEST_PRIME;
   } else {
      b = a + ERAT_UINT_MAX_SQRT;
   }

   n = ((b - a)+1)/CHAR_BIT;

   /*
      We are reusing that slice of heap for all segments.
      Support of multithreading would need a bit of work.
   */
   if (single_segment->content == NULL) {
      single_segment->content = (ERAT_UINT *) MP_MALLOC(n + sizeof(ERAT_UINT));
      if (single_segment->content == NULL) {
         return MP_MEM;
      }
      single_segment->alloc = n + sizeof(ERAT_UINT);
      single_segment->size = n * CHAR_BIT;
   }

   s_mp_erat_eratosthenes_segment(a, b, base_sieve, single_segment);
   *single_segment_a = a;

   return e;
}


/*
 * Simple Eratosthenes' sieve, starting at zero
 * Keeping odd numbers only as the single optimization
 */
void s_mp_erat_eratosthenes(mp_erat_single_sieve *bst)
{
   ERAT_UINT n, k, r, j;

   n = (ERAT_UINT)bst->size;
   r = s_mp_erat_isqrt(n);
   s_mp_mp_erat_sieve_setall(bst);

   for (k = 1; k < ((r - 1) / 2); k += 1) {
      if (s_mp_mp_erat_sieve_get_bit(bst, k)) {
         for (j = k * 2 * (k + 1); j < (n - 1) / 2; j += 2 * k + 1) {
            s_mp_mp_mp_erat_sieve_clear_bit(bst, j);
         }
      }
   }
}

ERAT_UINT s_mp_erat_nextprime(ERAT_UINT p, mp_erat_single_sieve *bst)
{
   ERAT_UINT ret;
   if (p <= 1) {
      return 2;
   }
   ret = s_mp_mp_erat_sieve_nextset(bst, ((p - 1) / 2) + 1);
   return 2 * ret + 1;
}

/*
 * TODO: Not memory optimized, it stores the even numbers, too.
 * Should be ok if the segments are small but needs to
 * be done at some time in the near future.
 *
 * Fill sieve "segment" of the range [a,b] from the basic sieve "base"
 * Default size of a segment (32-bit): 8192 bytes (plus some angst-allowance)
 */
void s_mp_erat_eratosthenes_segment(ERAT_UINT a, ERAT_UINT b, mp_erat_single_sieve *base, mp_erat_single_sieve *segment)
{
   ERAT_UINT r, j;
   ERAT_UINT p;

   r = s_mp_erat_isqrt(b);

   s_mp_mp_erat_sieve_setall(segment);
   p = 0;

   while (p <= r) {
      p = s_mp_erat_nextprime(p, base);
      j = p * p;
      if (j < a) {
         j = ((a + p - 1) / p) * p;
      }
      for (; j <= b; j += p) {
         /* j+=p can overflow, so check size of j in relation to a */
         if (j >= a) {
            s_mp_mp_mp_erat_sieve_clear_bit(segment, j - a);
         } else {
            break;
         }
      }
   }
}


#endif
