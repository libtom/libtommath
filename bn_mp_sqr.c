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

/* computes b = a*a */
int
mp_sqr (mp_int * a, mp_int * b)
{
  int       res;
  if (a->used > KARATSUBA_SQR_CUTOFF) {
    res = mp_karatsuba_sqr (a, b);
  } else {
    res = s_mp_sqr (a, b);
  }
  b->sign = MP_ZPOS;
  return res;
}
