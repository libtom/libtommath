#include <tommath.h>
#ifdef BN_MP_MUL_C
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

/* high level multiplication (handles sign) */
int mp_mul (mp_int * a, mp_int * b, mp_int * c)
{
  int     res, neg;
  neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;

  int     an, bn, tn;
  mp_int * t;
  an = a -> used;
  bn = b -> used;
  if (an > bn) {
    tn = an; an = bn; bn = tn;
    t = a; a = b; b = t;
  }
  /* now a->used <= b->used */

  /* use Toom-Cook? */
#ifdef BN_MP_TOOM_MUL_C
  if (a->used >= TOOM_MUL_CUTOFF) {
    if (2 * an <= bn) goto balance;
    res = mp_toom_mul(a, b, c);
  } else 
#endif
#ifdef BN_MP_KARATSUBA_MUL_C
  /* use Karatsuba? */
  if (a->used >= KARATSUBA_MUL_CUTOFF) {
    if (2 * an <= bn) goto balance;
    res = mp_karatsuba_mul (a, b, c);
  } else 
#endif
  {
    /* can we use the fast multiplier?
     *
     * The fast multiplier can be used if the output will 
     * have less than MP_WARRAY digits and the number of 
     * digits won't affect carry propagation
     */
    int     digs = a->used + b->used + 1;

#ifdef BN_FAST_S_MP_MUL_DIGS_C
    if ((digs < MP_WARRAY) &&
        a->used <= (1 << ((CHAR_BIT * sizeof (mp_word)) - (2 * DIGIT_BIT)))) {
      res = fast_s_mp_mul_digs (a, b, c, digs);
    } else 
#endif
#ifdef BN_S_MP_MUL_DIGS_C
      res = s_mp_mul (a, b, c); /* uses s_mp_mul_digs */
#else
      res = MP_VAL;
#endif

  }
ret:
  c->sign = (c->used > 0) ? neg : MP_ZPOS;
  return res;

balance:
  /* if a is much smaller than b
   * use balance multiplication
   * (the idea is taken from Ruby core)
   */
  res = mp_balance_mul(a, b, c);
  goto ret;
}
#endif

/* $Source$ */
/* $Revision$ */
/* $Date$ */
