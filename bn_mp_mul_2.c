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

    /* alias for source */
    tmpa = a->dp;
    
    /* alias for dest */
    tmpb = b->dp;

    /* carry */
    r = 0;
    for (x = 0; x < b->used; x++) {
    
      /* get what will be the *next* carry bit from the MSB of the current digit */
      rr = *tmpa >> (DIGIT_BIT - 1);
      
      /* now shift up this digit, add in the carry [from the previous] */
      *tmpb++ = ((*tmpa++ << 1) | r) & MP_MASK;
      
      /* copy the carry that would be from the source digit into the next iteration */
      r = rr;
    }

    /* new leading digit? */
    if (r != 0) {
      /* do we have to grow to accomodate the new digit? */
      if (b->alloc == b->used) {
	if ((res = mp_grow (b, b->used + 1)) != MP_OKAY) {
	  return res;
	}

	/* after the grow *tmpb is no longer valid so we have to reset it! 
	 * (this bug took me about 17 minutes to find...!)
	 */
	tmpb = b->dp + b->used;
      }
      /* add a MSB which is always 1 at this point */
      *tmpb = 1;
      ++b->used;
    }

    /* now zero any excess digits on the destination that we didn't write to */
    tmpb = b->dp + b->used;
    for (x = b->used; x < oldused; x++) {
      *tmpb++ = 0;
    }
  }
  b->sign = a->sign;
  return MP_OKAY;
}
