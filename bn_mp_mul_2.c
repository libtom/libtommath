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
 * Tom St Denis, tomstdenis@iahu.ca, http://libtommath.iahu.ca
 */
#include <tommath.h>

/* b = a*2 */
int
mp_mul_2 (mp_int * a, mp_int * b)
{
  int     x, res, oldused;

  /* Optimization: should copy and shift at the same time */

  if (b->alloc < a->used) {
    if ((res = mp_grow (b, a->used)) != MP_OKAY) {
      return res;
    }
  }

  oldused = b->used;
  b->used = a->used;

  /* shift any bit count < DIGIT_BIT */
  {
    register mp_digit r, rr, *tmpa, *tmpb;

    r = 0;
    tmpa = a->dp;
    tmpb = b->dp;
    for (x = 0; x < b->used; x++) {
      rr = *tmpa >> (DIGIT_BIT - 1);
      *tmpb++ = ((*tmpa++ << 1) | r) & MP_MASK;
      r = rr;
    }

    /* new leading digit? */
    if (r != 0) {
      if (b->alloc == b->used) {
	if ((res = mp_grow (b, b->used + 1)) != MP_OKAY) {
	  return res;
	}
      }
      /* add a MSB of 1 */
      *tmpb = 1;
      ++b->used;
    }

    tmpb = b->dp + b->used;
    for (x = b->used; x < oldused; x++) {
      *tmpb++ = 0;
    }
  }
  return MP_OKAY;
}
