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

/* multiply by a digit */
int
mp_mul_d (mp_int * a, mp_digit b, mp_int * c)
{
  int     res, pa, olduse;

  pa = a->used;
  if (c->alloc < pa + 1) {
    if ((res = mp_grow (c, pa + 1)) != MP_OKAY) {
      return res;
    }
  }

  olduse = c->used;
  c->used = pa + 1;

  {
    register mp_digit u, *tmpa, *tmpc;
    register mp_word r;
    register int ix;

    tmpc = c->dp + c->used;
    for (ix = c->used; ix < olduse; ix++) {
      *tmpc++ = 0;
    }

    tmpa = a->dp;
    tmpc = c->dp;

    u = 0;
    for (ix = 0; ix < pa; ix++) {
      r = ((mp_word) u) + ((mp_word) * tmpa++) * ((mp_word) b);
      *tmpc++ = (mp_digit) (r & ((mp_word) MP_MASK));
      u = (mp_digit) (r >> ((mp_word) DIGIT_BIT));
    }
    *tmpc = u;
  }

  mp_clamp (c);
  return MP_OKAY;
}
