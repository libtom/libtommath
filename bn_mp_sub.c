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

/* high level subtraction (handles signs) */
int
mp_sub (mp_int * a, mp_int * b, mp_int * c)
{
  int     sa, sb, res;


  sa = a->sign;
  sb = b->sign;

  /* handle four cases */
  if (sa == MP_ZPOS && sb == MP_ZPOS) {
    /* both positive, a - b, but if b>a then we do -(b - a) */
    if (mp_cmp_mag (a, b) == MP_LT) {
      /* b>a */
      res = s_mp_sub (b, a, c);
      c->sign = MP_NEG;
    } else {
      res = s_mp_sub (a, b, c);
      c->sign = MP_ZPOS;
    }
  } else if (sa == MP_ZPOS && sb == MP_NEG) {
    /* a - -b == a + b  */
    res = s_mp_add (a, b, c);
    c->sign = MP_ZPOS;
  } else if (sa == MP_NEG && sb == MP_ZPOS) {
    /* -a - b == -(a + b) */
    res = s_mp_add (a, b, c);
    c->sign = MP_NEG;
  } else {
    /* -a - -b == b - a, but if a>b == -(a - b) */
    if (mp_cmp_mag (a, b) == MP_GT) {
      res = s_mp_sub (a, b, c);
      c->sign = MP_NEG;
    } else {
      res = s_mp_sub (b, a, c);
      c->sign = MP_ZPOS;
    }
  }

  return res;
}
