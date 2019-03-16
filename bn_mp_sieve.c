#include "tommath_private.h"
#ifdef BN_MP_SIEVE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * SPDX-License-Identifier: Unlicense
 */


/* Segmented version of an Eratosthenes sieve */
#ifdef LTM_USE_EXTRA_FUNCTIONS

#define LTM_SIEVE_UINT_NUM_BITS (sizeof(LTM_SIEVE_UINT)*CHAR_BIT)

static void s_mp_sieve_setall(mp_sieve *bst);
static void s_mp_sieve_clear(mp_sieve *bst, LTM_SIEVE_UINT n);
static LTM_SIEVE_UINT s_mp_sieve_get(mp_sieve *bst, LTM_SIEVE_UINT n);
static LTM_SIEVE_UINT s_mp_sieve_nextset(mp_sieve *bst, LTM_SIEVE_UINT n);

static LTM_SIEVE_UINT isqrt(LTM_SIEVE_UINT n);

static void s_mp_eratosthenes(mp_sieve *bst);
static int  s_mp_eratosthenes_init(LTM_SIEVE_UINT n, mp_sieve **bst);

static void s_mp_eratosthenes_segment(LTM_SIEVE_UINT a, LTM_SIEVE_UINT b, mp_sieve *base,
                                      mp_sieve *segment);
static int s_mp_eratosthenes_segment_init(LTM_SIEVE_UINT a, LTM_SIEVE_UINT b, mp_sieve **base,
      mp_sieve **segment);
static void s_mp_eratosthenes_segment_clear(mp_sieve *segment, LTM_SIEVE_UINT *single_segment_a);

static int s_init_base_sieve(mp_sieve **base_sieve);
static int s_init_single_segment_with_start(LTM_SIEVE_UINT a, mp_sieve **base_sieve,
      mp_sieve **single_segment, LTM_SIEVE_UINT *single_segment_a);

static LTM_SIEVE_UINT s_nextprime(LTM_SIEVE_UINT p, mp_sieve *bst);


/* Manual memset() to avoid the inclusion of string.h */
static void s_mp_sieve_setall(mp_sieve *bst)
{
   LTM_SIEVE_UINT i, bs_size;
   bs_size = bst->alloc / sizeof(LTM_SIEVE_UINT);
   for (i = 0; i < bs_size; i++) {
      (bst)->content[i] = LTM_SIEVE_UINT_MAX;
   }
}

static void s_mp_sieve_clear(mp_sieve *bst, LTM_SIEVE_UINT n)
{
  ((*((bst)->content+(n/LTM_SIEVE_UINT_NUM_BITS)) 
           &= ~( 1lu<<( n % LTM_SIEVE_UINT_NUM_BITS ))));
}

static LTM_SIEVE_UINT s_mp_sieve_get(mp_sieve *bst, LTM_SIEVE_UINT n)
{
   return (((*((bst)->content+(n/LTM_SIEVE_UINT_NUM_BITS)) 
           & ( 1lu<<( n % LTM_SIEVE_UINT_NUM_BITS ))) != 0));
}

static LTM_SIEVE_UINT s_mp_sieve_nextset(mp_sieve *bst, LTM_SIEVE_UINT n)
{
   while ((n < ((bst)->size)) && (!s_mp_sieve_get(bst, n))) {
      n++;
   }
   return n;
}


/*
 * Integer square root, hardware style
 * Wikipedia calls it "Digit-by-digit calculation"
 * https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Digit-by-digit_calculation
 * This is the base 2 method described at
 * https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_(base_2)
 */
static LTM_SIEVE_UINT isqrt(LTM_SIEVE_UINT n)
{
   LTM_SIEVE_UINT s, rem, root;

   if (n < 1) {
      return 0;
   }
   /* highest power of four <= n */
   s = ((LTM_SIEVE_UINT) 1) << (LTM_SIEVE_UINT_NUM_BITS - 2);

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

/*
 * Set range_a_b to sqrt(LTM_SIEVE_UINT_MAX)
 * TODO: Make it const or put it in bncore.c because it is said to be faster
 * if the size of range_a_b fits into the L2-cache.
 * Not much difference on the author's machine for 32 bit but quite
 * a large one for 64 bit and large limits. YMMV, as always.
 * Please be aware that range_a_b is in bits, not bytes and memory
 * allocation rounds up and adds CHAR_BIT*sizeof(LTM_SIEVE_UINT) bits.
 */
#ifndef LTM_SIEVE_RANGE_A_B
#if ( (defined MP_64BIT) && (defined LTM_SIEVE_USE_LARGE_SIEVE) )
#define LTM_SIEVE_RANGE_A_B  0x400000uL
#else
#define LTM_SIEVE_RANGE_A_B ((LTM_SIEVE_UINT) LTM_SIEVE_UINT_MAX_SQRT)
#endif
#endif
#define LTM_SIEVE_BASE_SIEVE_SIZE  ((LTM_SIEVE_UINT)LTM_SIEVE_UINT_MAX_SQRT)

/* TODO: Some redundant code below, needs a clean-up */


/*
 * Initiate a sieve that stores the odd numbers only:
 * allocate memory, set actual size and allocated size and fill it
 */
static int s_mp_eratosthenes_init(LTM_SIEVE_UINT n, mp_sieve **bst)
{
   n = (n - 1) / 2;
   *bst = malloc(sizeof(mp_sieve));
   if (bst == NULL) {
      return MP_MEM;
   }
   (*bst)->content =
      malloc((n + sizeof(LTM_SIEVE_UINT)) / sizeof(LTM_SIEVE_UINT) + sizeof(LTM_SIEVE_UINT));
   if ((*bst)->content == NULL) {
      return MP_MEM;
   }

   (*bst)->alloc =
      (n + sizeof(LTM_SIEVE_UINT)) / sizeof(LTM_SIEVE_UINT) + sizeof(LTM_SIEVE_UINT);
   (*bst)->size = 2 * n + 1;

   s_mp_eratosthenes(*bst);
   return MP_OKAY;
}

/*
 * Simple Eratosthenes' sieve, starting at zero
 * Keeping odd primes only as the single optimization
 */
static void s_mp_eratosthenes(mp_sieve * bst)
{
  LTM_SIEVE_UINT n, k, r, j;

  n = (bst)->size;
  r = isqrt(n);
  s_mp_sieve_setall(bst);
  for (k = 1; k < ((r - 1) / 2); k += 1) {
    if (s_mp_sieve_get(bst, k)) {
      for (j = k * 2 * (k + 1); j < (n - 1) / 2; j += 2 * k + 1) {
        s_mp_sieve_clear(bst, j);
      }
    }
  }
}

/*
 * For a sieve that has only the odd numbers stored.
 * It returns next prime even if p is prime in contrast to
 * e.g.: Pari/GP's nextprime() function
 * Used internally for the segment to get the primes from
 * the base sieve.
 */
static LTM_SIEVE_UINT s_nextprime(LTM_SIEVE_UINT p, mp_sieve * bst)
{
  LTM_SIEVE_UINT ret;
  if (p <= 1)
    return 2;
  ret = s_mp_sieve_nextset(bst, ((p - 1) / 2) + 1);
  return 2 * ret + 1;
}

/*
 * Init sieve "segment" of the range [a,b]:
 * allocate memory, set actual size and allocated size and fill it from sieve "base"
 * TODO: merge with s_mp_eratosthenes_init()
 */
static int s_mp_eratosthenes_segment_init(LTM_SIEVE_UINT a, LTM_SIEVE_UINT b,
                                          mp_sieve ** segment, mp_sieve ** base)
{
  LTM_SIEVE_UINT n;

  n = b - a;

  *segment = malloc(sizeof(mp_sieve));
  if (segment == NULL) {
    return MP_MEM;
  }
  (*segment)->content =
      malloc((n + sizeof(LTM_SIEVE_UINT)) / sizeof(LTM_SIEVE_UINT) +
             sizeof(LTM_SIEVE_UINT));
  if ((*segment)->content == NULL) {
    return MP_MEM;
  }
  (*segment)->alloc =
      (n + sizeof(LTM_SIEVE_UINT)) / sizeof(LTM_SIEVE_UINT) +
      sizeof(LTM_SIEVE_UINT);
  (*segment)->size = n;

  s_mp_eratosthenes_segment(a, b, *base, *segment);
  return MP_OKAY;
}

/*
 * Clear sieve "segment"
 * free memory and reset "single_segment_a"
 */
static void s_mp_eratosthenes_segment_clear(mp_sieve * segment,
                                            LTM_SIEVE_UINT * single_segment_a)
{
  if (segment != NULL) {
    if (segment->content != NULL) {
      free(segment->content);
    }
    free(segment);
    *single_segment_a = 0;
  }
}


/*
 * TODO: Not memory optimized, it stores the even numbers, too.
 * Should be ok if the segments are small but needs to
 * be done at some time in the near future.
 * 
 * Fill sieve "segment" of the range [a,b] from the basic sieve "base"
 */
static void s_mp_eratosthenes_segment(LTM_SIEVE_UINT a, LTM_SIEVE_UINT b,
                                      mp_sieve * base, mp_sieve * segment)
{
  LTM_SIEVE_UINT r, j, i;
  LTM_SIEVE_UINT p;
  r = isqrt(b);
  s_mp_sieve_setall(segment);
  p = 0;
  for (i = 0; p <= r; i++) {
    p = s_nextprime(p, base);
    j = p * p;
    if (j < a) {
      j = ((a + p - 1) / p) * p;
    }
    for (; j <= b; j += p) {
      /* j+=p can overflow */
      if (j >= a) {
        s_mp_sieve_clear(segment, j - a);
      } else {
        break;
      }
    }
  }
}


/* Build a basic sieve with the largest reasonable size */
static int s_init_base_sieve(mp_sieve ** base_sieve)
{
  LTM_SIEVE_UINT n;
  /* 
   * That is a risky idea. If range_a_b is smaller than sqrt(n_max)
   * the whole thing stops working!
   */
  /* n = range_a_b; */
  n = LTM_SIEVE_BASE_SIEVE_SIZE;
  return s_mp_eratosthenes_init(n, base_sieve);
}

/* 
 * Build a segment sieve with the largest reasonable size. "a" is the start of
 * the sieve Size is MIN(range_a_b,LTM_SIEVE_UINT_MAX-a)
 */
static int s_init_single_segment_with_start(LTM_SIEVE_UINT a,
                                            mp_sieve ** base_sieve,
                                            mp_sieve ** single_segment,
                                            LTM_SIEVE_UINT * single_segment_a)
{
  LTM_SIEVE_UINT b;
  int e = MP_OKAY;

  /* last segment might not fit, depending on size of range_a_b */
  if (a > (LTM_SIEVE_UINT_MAX - LTM_SIEVE_RANGE_A_B)) {
    b = LTM_SIEVE_UINT_MAX - 1;
  } else {
    b = a + LTM_SIEVE_RANGE_A_B;
  }
  if ((e =
       s_mp_eratosthenes_segment_init(a, b, single_segment,
                                      base_sieve)) != MP_OKAY) {
    return e;
  }
  *single_segment_a = a;
  return e;
}

/*
 * Sets "result" to one if n is prime or zero respectively.
 * Also sets "result" to zero in case of error.
 * Worst case runtime is: building a base sieve and a segment and
 * search the segment
 */
int mp_is_small_prime(LTM_SIEVE_UINT n, LTM_SIEVE_UINT *result, mp_sieve **base_sieve,
                      mp_sieve **single_segment, LTM_SIEVE_UINT *single_segment_a)
{
   int e = MP_OKAY;
   LTM_SIEVE_UINT a = 0, b = 0;

   if (n < 2) {
      *result = 0;
      return e;
   }

   if ((n & 1) == 0) {
      *result = 0;
      return e;
   }

   /* neither of 2^16-x, 2^32-x, or 2^64-x are prime for 0<=x<=4 */
   if (n >= (LTM_SIEVE_UINT_MAX - 3)) {
      *result = 0;
      return e;
   }

   if ((*base_sieve) == NULL) {
      if ((e = s_init_base_sieve(base_sieve)) != MP_OKAY) {
         *result = 0;
         return e;
      }
   }

   /* No need to generate a segment if n is in the base sieve */
   if (n < LTM_SIEVE_BASE_SIEVE_SIZE) {
      /* might have been a small sieve, so check size of sieve first */
      if (n < ((*base_sieve)->size)) {
         *result = s_mp_sieve_get(*base_sieve, (n - 1) / 2);
         return e;
      }
   }

   /* no further shortcuts to apply, build and search a segment */

   /*
    * TODO: if base_sieve is < sqrt(LTM_SIEVE_UINT_MAX) it is not possible to get
    * all primes <= LTM_SIEVE_UINT_MAX so add check if n > size(base_sieve)^2 and either
    * a. make size(base_sieve) = sqrt(LTM_SIEVE_UINT_MAX)
    * b. make size(base_sieve) = 2 * size(base_sieve)
    * with 2 * size(base_sieve) <= sqrt(LTM_SIEVE_UINT_MAX)
    * c. give up and return MP_VAL
    */
   /* we have a segment and may be able to use it */
   if ((*single_segment) != NULL) {
      a = *single_segment_a;
      /* last segment may not fit into range_a_b */
      if (a > (LTM_SIEVE_UINT_MAX - LTM_SIEVE_RANGE_A_B)) {
         b = LTM_SIEVE_UINT_MAX - 1;
      } else {
         b = a + LTM_SIEVE_RANGE_A_B;
      }
      /* check if n is inside the bounds of the segment */
      if (n >= a && n <= b) {
         *result = s_mp_sieve_get(*single_segment, n - a);
         return e;
      }
      /* get a clean slate */
      else {
         s_mp_eratosthenes_segment_clear(*single_segment, single_segment_a);
      }
   }

   /*
    * A bit of heuristics ( "heuristics" is a more pretentious word for the
    * commonly known expression "wild guess")
    * Based on the vague idea of the assumption that most sieves get used for
    * sequential series of primes or a single test here and there, but not
    * for massive amounts of random requests.
    */
   if (n > a) {
      if (n > (LTM_SIEVE_UINT_MAX - LTM_SIEVE_RANGE_A_B)) {
         a = LTM_SIEVE_UINT_MAX - LTM_SIEVE_RANGE_A_B;
      } else {
         a = n;
      }
   } else {
      if (n < LTM_SIEVE_RANGE_A_B) {
         a = LTM_SIEVE_BASE_SIEVE_SIZE - 1;
      } else {
         /* TODO: there should be no need for an overlap, check */
         a = n - (LTM_SIEVE_RANGE_A_B - 2);
      }
   }
   if ((e = s_init_single_segment_with_start(a, base_sieve, single_segment,single_segment_a)) != MP_OKAY) {
      *result = 0;
      return e;
   }
   /* finally, check for primality */
   *result = s_mp_sieve_get(*single_segment, n - a);
   return e;
}

/*
 * Mimics behaviour of Pari/GP's nextprime(n)
 * If n is prime set *result to n else set *result to first prime > n
 * and 0 in case of error
 */
int mp_next_small_prime(LTM_SIEVE_UINT n, LTM_SIEVE_UINT *result, mp_sieve **base_sieve,
                        mp_sieve **single_segment, LTM_SIEVE_UINT *single_segment_a)
{
   LTM_SIEVE_UINT ret = 0;
   int e = MP_OKAY;

   if (n < 2) {
      *result = 2;
      return e;
   }

   for (; ret == 0 && n != 0; n++) {
      if (n > LTM_SIEVE_BIGGEST_PRIME) {
         return LTM_SIEVE_MAX_REACHED;
      }
      /* just call mp_is_small_prime(), it does all of the heavy work */
      if ((e = mp_is_small_prime(n, &ret, base_sieve, single_segment, single_segment_a)) != MP_OKAY) {
         *result = 0;
         return e;
      }
   }
   *result = n - 1;
   return e;
}

/*
 * Mimics behaviour of Pari/GP's precprime(n)
 * If n is prime set *result to n else set *result to first prime < n
 * and 0 in case of error
 */
int mp_prec_small_prime(LTM_SIEVE_UINT n, LTM_SIEVE_UINT *result, mp_sieve **base_sieve,
                        mp_sieve **single_segment, LTM_SIEVE_UINT *single_segment_a)
{
   LTM_SIEVE_UINT ret = 0;
   int e = MP_OKAY;

   if (n == 2) {
      *result = 2;
      return e;
   }

   if (n < 2) {
      *result = 0;
      return e;
   }

   for (; ret == 0; n--) {
      if ((e = mp_is_small_prime(n, &ret, base_sieve, single_segment,single_segment_a)) != MP_OKAY) {
         *result = 0;
         return e;
      }
   }
   *result = n + 1;

   return e;
}

int mp_sieve_init(mp_sieve *sieve)
{
   sieve = malloc(sizeof(mp_sieve));
   if (sieve == NULL) {
      return MP_MEM;
   }
   sieve->content = NULL;
   sieve->alloc = 0;
   sieve->size = 0;
   return MP_OKAY;
}
void mp_sieve_clear(mp_sieve *sieve)
{
   if (sieve != NULL) {
      free(sieve->content);
      free(sieve);
   }
}


int mp_small_prime_array(LTM_SIEVE_UINT start, LTM_SIEVE_UINT end,
                         LTM_SIEVE_UINT **prime_array,
                         LTM_SIEVE_UINT *array_length, mp_sieve **base_sieve,
                         mp_sieve **single_segment,
                         LTM_SIEVE_UINT *single_segment_a)
{

   LTM_SIEVE_UINT k, ret, approx_pi, allocated = 0;
   LTM_SIEVE_UINT *tmp_prime_array;
   int e = MP_OKAY;

   if (start > end) {
      return MP_VAL;
   }
   if (start > LTM_SIEVE_BIGGEST_PRIME) {
      return MP_VAL;
   }

   if (end > LTM_SIEVE_BIGGEST_PRIME) {
      end = LTM_SIEVE_BIGGEST_PRIME;
   }

#ifndef LTM_SIEVE_GROWTH
#   define LTM_SIEVE_GROWTH 128
#endif
   approx_pi = LTM_SIEVE_GROWTH;
   allocated = LTM_SIEVE_GROWTH;

   *prime_array = malloc((size_t)(approx_pi) * sizeof(LTM_SIEVE_UINT));
   if (*prime_array == NULL) {
      return MP_MEM;
   }

   if (start > 0) {
      start--;
   }
   *array_length = 0u;
   for (k = start, ret = 0; ret < end; k = ret) {
      if ((e = mp_next_small_prime(k + 1, &ret, base_sieve, single_segment, single_segment_a)) != MP_OKAY) {
         if (e == LTM_SIEVE_MAX_REACHED) {
            (*prime_array)[*array_length] = ret;
            break;
         }
         goto LTM_SIEVE_END;
      }
      /* The check if the prime is below the given limit
       * cannot be done in the for-loop conditions because if 
       * it could we wouldn't need the sieve in the first place.
       */
      if (ret <= end) {
         if ((*array_length) <= allocated) {
            tmp_prime_array = realloc((*prime_array), ((*array_length) + LTM_SIEVE_GROWTH) * sizeof(LTM_SIEVE_UINT));
            if (tmp_prime_array == NULL) {
               return MP_MEM;
            }
            *prime_array = tmp_prime_array;
            allocated = (*array_length) + LTM_SIEVE_GROWTH;
         }
         (*prime_array)[(*array_length)++] = ret;
      }
      if (ret == LTM_SIEVE_BIGGEST_PRIME) {
         break;
      }
   }
   tmp_prime_array = realloc((*prime_array), ((*array_length) + 1) * sizeof(LTM_SIEVE_UINT));
   if (tmp_prime_array == NULL) {
      return MP_MEM;
   }
   *prime_array = tmp_prime_array;

LTM_SIEVE_END:
   return e;
}
#endif /* LTM_USE_EXTRA_FUNCTIONS */
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
