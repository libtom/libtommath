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
  mp_digit  r, rr;
  int       x, res;


  /* copy */
  if ((res = mp_copy (a, b)) != MP_OKAY) {
    return res;
  }

  r = 0;
  for (x = b->used - 1; x >= 0; x--) {
    rr = b->dp[x] & 1;
    b->dp[x] = (b->dp[x] >> 1) | (r << (DIGIT_BIT - 1));
    r = rr;
  }
  mp_clamp (b);
  return MP_OKAY;
}
