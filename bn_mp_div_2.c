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

/* b = a/2 */
int
mp_div_2 (mp_int * a, mp_int * b)
{
  int     x, res, oldused;

  /* copy */
  if (b->alloc < a->used) {
    if ((res = mp_grow (b, a->used)) != MP_OKAY) {
      return res;
    }
  }

  oldused = b->used;
  b->used = a->used;
  {
    register mp_digit r, rr, *tmpa, *tmpb;

    tmpa = a->dp + b->used - 1;
    tmpb = b->dp + b->used - 1;
    r = 0;
    for (x = b->used - 1; x >= 0; x--) {
      rr = *tmpa & 1;
      *tmpb-- = (*tmpa-- >> 1) | (r << (DIGIT_BIT - 1));
      r = rr;
    }

    tmpb = b->dp + b->used;
    for (x = b->used; x < oldused; x++) {
      *tmpb++ = 0;
    }
  }
  mp_clamp (b);
  return MP_OKAY;
}
