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
 * Tom St Denis, tomstdenis@iahu.ca, http://math.libtomcrypt.org
 */
#include <tommath.h>

/* single digit subtraction */
int
mp_sub_d (mp_int * a, mp_digit b, mp_int * c)
{
  mp_digit *tmpa, *tmpc, mu;
  int       res, ix, oldused;

  /* grow c as required */
  if (c->alloc < a->used + 1) {
     if ((res = mp_grow(c, a->used + 1)) != MP_OKAY) {
        return res;
     }
  }

  /* if a is negative just do an unsigned
   * addition [with fudged signs]
   */
  if (a->sign == MP_NEG) {
     a->sign = MP_ZPOS;
     res     = mp_add_d(a, b, c);
     a->sign = c->sign = MP_NEG;
     return res;
  }

  /* setup regs */
  oldused = c->used;
  tmpa    = a->dp;
  tmpc    = c->dp;

  /* if a <= b simply fix the single digit */
  if ((a->used == 1 && a->dp[0] <= b) || a->used == 0) {
     *tmpc++ = b - *tmpa;
     ix      = 1;

     /* negative/1digit */
     c->sign = MP_NEG;
     c->used = 1;
  } else {
     /* positive/size */
     c->sign = MP_ZPOS;
     c->used = a->used;

     /* subtract first digit */
     *tmpc    = *tmpa++ - b;
     mu       = *tmpc >> (sizeof(mp_digit) * CHAR_BIT - 1);
     *tmpc++ &= MP_MASK;

     /* handle rest of the digits */
     for (ix = 1; ix < a->used; ix++) {
        *tmpc    = *tmpa++ - mu;
        mu       = *tmpc >> (sizeof(mp_digit) * CHAR_BIT - 1);
        *tmpc++ &= MP_MASK;
     }
  }

  for (; ix < oldused; ix++) {
     *tmpc++ = 0;
  }
  mp_clamp(c);
  return MP_OKAY;
}

