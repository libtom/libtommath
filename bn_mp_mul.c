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

/* high level multiplication (handles sign) */
int
mp_mul (mp_int * a, mp_int * b, mp_int * c)
{
  int       res, neg;
  neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;
  if (MIN (a->used, b->used) > KARATSUBA_MUL_CUTOFF) {
    res = mp_karatsuba_mul (a, b, c);
  } else {
    res = s_mp_mul (a, b, c);
  }
  c->sign = neg;
  return res;
}
