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

/* low level subtraction (assumes a > b), HAC pp.595 Algorithm 14.9 */
int
s_mp_sub (mp_int * a, mp_int * b, mp_int * c)
{
  int       olduse, res, min, max, i;
  mp_digit  u;


  /* find sizes */
  min = b->used;
  max = a->used;

  /* init result */
  if (c->alloc < max) {
    if ((res = mp_grow (c, max)) != MP_OKAY) {
      return res;
    }
  }
  olduse = c->used;
  c->used = max;

  /* sub digits from lower part */

  /* set carry to zero */
  u = 0;
  for (i = 0; i < min; i++) {
    /* T[i] = A[i] - B[i] - U */
    c->dp[i] = a->dp[i] - (b->dp[i] + u);

    /* U = carry bit of T[i] */
    u = (c->dp[i] >> DIGIT_BIT) & 1;

    /* Clear carry from T[i] */
    c->dp[i] &= MP_MASK;
  }

  /* now copy higher words if any, e.g. if A has more digits than B  */
  if (min != max) {
    for (; i < max; i++) {
      /* T[i] = A[i] - U */
      c->dp[i] = a->dp[i] - u;

      /* U = carry bit of T[i] */
      u = (c->dp[i] >> DIGIT_BIT) & 1;

      /* Clear carry from T[i] */
      c->dp[i] &= MP_MASK;
    }
  }

  /* clear digits above used (since we may not have grown result above) */
  for (i = c->used; i < olduse; i++) {
    c->dp[i] = 0;
  }

  mp_clamp (c);
  return MP_OKAY;
}
