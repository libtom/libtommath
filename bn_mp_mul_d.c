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
  int       res, pa, ix;
  mp_word   r;
  mp_digit  u;
  mp_int    t;


  pa = a->used;
  if ((res = mp_init_size (&t, pa + 2)) != MP_OKAY) {
    return res;
  }
  t.used = pa + 2;

  u = 0;
  for (ix = 0; ix < pa; ix++) {
    r = ((mp_word) u) + ((mp_word) a->dp[ix]) * ((mp_word) b);
    t.dp[ix] = (mp_digit) (r & ((mp_word) MP_MASK));
    u = (mp_digit) (r >> ((mp_word) DIGIT_BIT));
  }
  t.dp[ix] = u;

  t.sign = a->sign;
  mp_clamp (&t);
  mp_exch (&t, c);
  mp_clear (&t);
  return MP_OKAY;
}
