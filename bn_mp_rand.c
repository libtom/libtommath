#include <tommath_private.h>
#ifdef BN_MP_RAND_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tstdenis82@gmail.com, http://libtom.org
 */

/* makes a pseudo-random int of a given size */
static mp_digit mp_gen_random(void)
{
  mp_digit d;
  d = ((mp_digit) abs (MP_GEN_RANDOM()));
#if MP_DIGIT_BIT > 32
  d <<= 32;
  d |= ((mp_digit) abs (MP_GEN_RANDOM()));
#endif
  d &= MP_MASK;
  return d;
}

int
mp_rand (mp_int * a, int digits)
{
  int     res;
  mp_digit d;

  mp_zero (a);
  if (digits <= 0) {
    return MP_OKAY;
  }

  /* first place a random non-zero digit */
  do {
    d = mp_gen_random();
  } while (d == 0);

  if ((res = mp_add_d (a, d, a)) != MP_OKAY) {
    return res;
  }

  while (--digits > 0) {
    if ((res = mp_lshd (a, 1)) != MP_OKAY) {
      return res;
    }

    if ((res = mp_add_d (a, mp_gen_random(), a)) != MP_OKAY) {
      return res;
    }
  }

  return MP_OKAY;
}
#endif

/* $Source$ */
/* $Revision$ */
/* $Date$ */
