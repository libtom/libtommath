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

/* shift left a certain amount of digits */
int
mp_lshd (mp_int * a, int b)
{
  int     x, res;


  /* if its less than zero return */
  if (b <= 0) {
    return MP_OKAY;
  }

  /* grow to fit the new digits */
  if ((res = mp_grow (a, a->used + b)) != MP_OKAY) {
    return res;
  }

  /* increment the used by the shift amount than copy upwards */
  a->used += b;
  for (x = a->used - 1; x >= b; x--) {
    a->dp[x] = a->dp[x - b];
  }

  /* zero the lower digits */
  for (x = 0; x < b; x++) {
    a->dp[x] = 0;
  }
  mp_clamp (a);
  return MP_OKAY;
}
