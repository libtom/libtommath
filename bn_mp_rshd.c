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

/* shift right a certain amount of digits */
void
mp_rshd (mp_int * a, int b)
{
  int     x;


  /* if b <= 0 then ignore it */
  if (b <= 0) {
    return;
  }

  /* if b > used then simply zero it and return */
  if (a->used < b) {
    mp_zero (a);
    return;
  }

  /* shift the digits down */
  for (x = 0; x < (a->used - b); x++) {
    a->dp[x] = a->dp[x + b];
  }

  /* zero the top digits */
  for (; x < a->used; x++) {
    a->dp[x] = 0;
  }
  mp_clamp (a);
}
