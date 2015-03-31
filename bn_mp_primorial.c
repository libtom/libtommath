#include <tommath.h>
#ifdef BN_MP_PRIMORIAL_C
#   include <stdio.h>
#   include <stdlib.h>
#   include <stdint.h>
#   include <string.h>
#   include <inttypes.h>
#   include <limits.h>
#   include <math.h>

/* It's called Hallek's method but it has many inventors*/
static uint32_t isqrt(uint32_t n)
{
  uint32_t s, rem, root;
  if (n < 1)
    return 0;
  /* This is actually the highest square but it goes
   * downward from this, quite fast */
  s = 1 << 30;
  rem = n;
  root = 0;
  while (s > 0) {
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

uint32_t mp_bitset_nextset(mp_bitset_t * bst, uint32_t n)
{
  while ((n < mp_bitset_size(bst)) && (!mp_bitset_get(bst, n))) {
    n++;
  }
  return n;
}

uint32_t mp_bitset_prevset(mp_bitset_t * bst, uint32_t n)
{
  while (n > 1 && (!mp_bitset_get(bst, n))) {
    n--;
  }
  return n;
}

/* 
 * Standard method, quite antique now, but good enough for the handfull
 * of primes needed here.
 */
void mp_eratosthenes(mp_bitset_t * bst)
{
  uint32_t n, k, r, j;

  mp_bitset_setall(bst);
  mp_bitset_clear(bst, 0);
  mp_bitset_clear(bst, 1);

  n = mp_bitset_size(bst);
  r = isqrt(n);
  for (k = 4; k < n; k += 2)
    mp_bitset_clear(bst, k);
  k = 0;
  while ((k = mp_bitset_nextset(bst, k + 1)) < n) {
    if (k > r) {
      break;
    }
    for (j = k * k; j < n; j += k * 2) {
      mp_bitset_clear(bst, j);
    }
  }
}

#   define LN_113 1.25505871293247979696870747618124469168920275806274
unsigned long *mp_fill_prime_list(unsigned long start, unsigned long stop,
				  unsigned long *R)
{

  mp_bitset_t *bst;
  uint32_t n, k, j, i;
  unsigned long pix, *list;

  pix = (unsigned long) (LN_113 * stop / log(stop)) + 2;

  list = malloc(pix * sizeof(unsigned long));
  if (list == NULL) {
    return 0;
  }

  bst = malloc(sizeof(mp_bitset_t));
  mp_bitset_alloc(bst, stop);

  mp_bitset_setall(bst);
  mp_bitset_clear(bst, 0);
  mp_bitset_clear(bst, 1);

  n = mp_bitset_size(bst);
  for (k = 4; k < n; k += 2)
    mp_bitset_clear(bst, k);
  k = 0;
  i = 0;
  while ((k = mp_bitset_nextset(bst, k + 1)) < n) {
    if (k >= start) {
      list[i++] = k;
    }
    for (j = k * k; j < n; j += k * 2) {
      mp_bitset_clear(bst, j);

    }
  }
  mp_bitset_free(bst);
  list = realloc(list, sizeof(unsigned long) * (i + 1));
  *R = i;
  return list;
}

#   define MP_DIGIT_SIZE (1L<<DIGIT_BIT)
#   define MP_DIGIT_BIT_HALF (DIGIT_BIT>>1)
#   define MP_DIGIT_HALF (1L<< MP_DIGIT_BIT_HALF )
#   define MP_DIGIT_MASK (MP_DIGIT_HALF-1)

static int primorial__lowlevel(unsigned long *array, unsigned long n,
			       mp_int * result)
{
  unsigned long int i, first_half, second_half;
  mp_int tmp;
  int e;

  if (n == 0) {
    mp_set(result, 1);
    return MP_OKAY;
  }
  // Do the rest linearily. Faster for primorials at least,  but YMMV
  if (n <= 64) {
    mp_set(result, array[0]);
    for (i = 1; i < n; i++)
      mp_mul_d(result, array[i], result);
    return MP_OKAY;
  }

  first_half = n / 2;
  second_half = n - first_half;
  if ((e = primorial__lowlevel(array, second_half, result)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_init(&tmp)) != MP_OKAY) {
    return e;
  }
  if ((e =
       primorial__lowlevel(array + second_half, first_half, &tmp)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_mul(result, &tmp, result)) != MP_OKAY) {
    return e;
  }
  mp_clear(&tmp);
  return MP_OKAY;
}

int mp_primorial(unsigned long a, unsigned long b, mp_int * result)
{
  unsigned long r = 0;
  unsigned long *primelist;
  int e;
  primelist = mp_fill_prime_list(a + 1, b + 1, &r);
  if ((e = primorial__lowlevel(primelist, r, result)) != MP_OKAY) {
    return e;
  }
  return MP_OKAY;
}

#endif

