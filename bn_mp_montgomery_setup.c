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

/* setups the montgomery reduction stuff */
int
mp_montgomery_setup (mp_int * a, mp_digit * mp)
{
  mp_int    t, tt;
  int       res;

  if ((res = mp_init (&t)) != MP_OKAY) {
    return res;
  }

  if ((res = mp_init (&tt)) != MP_OKAY) {
    goto __T;
  }

  /* tt = b */
  tt.dp[0] = 0;
  tt.dp[1] = 1;
  tt.used = 2;

  /* t = m mod b */
  t.dp[0] = a->dp[0];
  t.used = 1;

  /* t = 1/m mod b */
  if ((res = mp_invmod (&t, &tt, &t)) != MP_OKAY) {
    goto __TT;
  }

  /* t = -1/m mod b */
  *mp = ((mp_digit) 1 << ((mp_digit) DIGIT_BIT)) - t.dp[0];

  res = MP_OKAY;
__TT:mp_clear (&tt);
__T:mp_clear (&t);
  return res;
}
