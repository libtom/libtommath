/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is library that provides for multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library is designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@iahu.ca, http://math.libtomcrypt.org
 */
#include <tommath.h>

/* shift left by a certain bit count */
int
mp_mul_2d (mp_int * a, int b, mp_int * c)
{
  mp_digit d, r, rr;
  int     x, res;

  /* copy */
  if ((res = mp_copy (a, c)) != MP_OKAY) {
    return res;
  }

  if ((res = mp_grow (c, c->used + b / DIGIT_BIT + 1)) != MP_OKAY) {
    return res;
  }

  /* shift by as many digits in the bit count */
  if (b >= DIGIT_BIT) {
    if ((res = mp_lshd (c, b / DIGIT_BIT)) != MP_OKAY) {
      return res;
    }
  }
  c->used = c->alloc;

  /* shift any bit count < DIGIT_BIT */
  d = (mp_digit) (b % DIGIT_BIT);
  if (d != 0) {
    register mp_digit *tmpc, mask;
    
    /* bitmask for carries */
    mask = (1U << d) - 1U;
    
    /* alias */
    tmpc = c->dp;
    
    /* carry */
    r    = 0;
    for (x = 0; x < c->used; x++) {
      /* get the higher bits of the current word */
      rr = (*tmpc >> (DIGIT_BIT - d)) & mask;

      /* shift the current word and OR in the carry */
      *tmpc = ((*tmpc << d) | r) & MP_MASK;
      ++tmpc;

      /* set the carry to the carry bits of the current word */
      r = rr;
    }
  }
  mp_clamp (c);
  return MP_OKAY;
}
