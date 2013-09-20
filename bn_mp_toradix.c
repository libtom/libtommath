#include <tommath.h>
#ifdef BN_MP_TORADIX_C
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
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */

/* stores a bignum as a ASCII string in a given radix (2..64) */
int mp_toradix (mp_int * a, char *str, int radix)
{
  int     res, digits_at_a_time;
  mp_int  t;
  mp_digit d, divide_by;
  char   *_s = str;
  static int pow_of_radix[64] = {0};
  static mp_digit radix_to_pow[64];

  /* check range of the radix */
  if (radix < 2 || radix > 64) {
    return MP_VAL;
  }

  /* quick out if its zero */
  if (mp_iszero(a) == 1) {
     *str++ = '0';
     *str = '\0';
     return MP_OKAY;
  }

  /* Find the largest power of radix which doesn't exceed MP_DIGIT_MAX */
  if( pow_of_radix[radix-1] ) {
    digits = pow_of_radix[radix-1];
    divide_by = radix_to_pow[radix-1];
  } else {
    mp_digit test = radix;
    digits_at_a_time = 0;
    do {
      divide_by = test;
      ++digits_at_a_time;
      test *= radix;
    } while( test <= MP_DIGIT_MAX && test > divide_by );
    pow_of_radix[radix-1] = digits;
    radix_to_pow[radix-1] = divide_by;
  }

  if ((res = mp_init_copy (&t, a)) != MP_OKAY) {
    return res;
  }

  /* if it is negative output a - */
  if (t.sign == MP_NEG) {
    ++_s;
    *str++ = '-';
    t.sign = MP_ZPOS;
  }

  if ((res = mp_div_d (&t, divide_by, &t, &d)) != MP_OKAY) {
    mp_clear (&t);
    return res;
  }
  while (mp_iszero (&t) == 0) {
    int digits;
    for( digits = digits_at_a_time; digits > 1; --digits ) {
      *str++ = mp_s_rmap[ d % radix ];
      d /= radix;
    }
    *str++ = mp_s_rmap[d];
    if ((res = mp_div_d (&t, divide_by, &t, &d)) != MP_OKAY) {
      mp_clear (&t);
      return res;
    }
  }
  while( d >= radix ) {
    *str++ = mp_s_rmap[ d % radix ];
    d /= radix;
  }
  if( d ) {
    *str++ = mp_s_rmap[ d ];
  }

  /* reverse the digits of the string.  In this case _s points
   * to the first digit [exluding the sign] of the number]
   */
  bn_reverse ((unsigned char *)_s, str - _s);

  /* append a NULL so the string is properly terminated */
  *str = '\0';

  mp_clear (&t);
  return MP_OKAY;
}

#endif

/* $Source$ */
/* $Revision$ */
/* $Date$ */
