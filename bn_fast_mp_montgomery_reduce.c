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

/* computes xR^-1 == x (mod N) via Montgomery Reduction (comba) */
int
fast_mp_montgomery_reduce (mp_int * a, mp_int * m, mp_digit mp)
{
  int       ix, res, olduse;
  mp_word   W[512];

  /* get old used count */
  olduse = a->used;

  /* grow a as required */
  if (a->alloc < m->used + 1) {
    if ((res = mp_grow (a, m->used + 1)) != MP_OKAY) {
      return res;
    }
  }

  /* copy the digits of a */
  for (ix = 0; ix < a->used; ix++) {
    W[ix] = a->dp[ix];
  }

  /* zero the high words */
  for (; ix < m->used * 2 + 1; ix++) {
    W[ix] = 0;
  }

  for (ix = 0; ix < m->used; ix++) {
    /* ui = ai * m' mod b
     *
     * We avoid a double precision multiplication (which isn't required)
     * by casting the value down to a mp_digit.  Note this requires that W[ix-1] have
     * the carry cleared (see after the inner loop)
     */
    register mp_digit ui;
    ui = (((mp_digit) (W[ix] & MP_MASK)) * mp) & MP_MASK;

    /* a = a + ui * m * b^i
     *
     * This is computed in place and on the fly.  The multiplication
     * by b^i is handled by offseting which columns the results
     * are added to.
     *
     * Note the comba method normally doesn't handle carries in the inner loop
     * In this case we fix the carry from the previous column since the Montgomery
     * reduction requires digits of the result (so far) [see above] to work.  This is
     * handled by fixing up one carry after the inner loop.  The carry fixups are done
     * in order so after these loops the first m->used words of W[] have the carries
     * fixed
     */
    {
      register int iy;
      register mp_digit *tmpx;
      register mp_word *_W;

      /* aliases */
      tmpx = m->dp;
      _W = W + ix;

      /* inner loop */
      for (iy = 0; iy < m->used; iy++) {
	*_W++ += ((mp_word) ui) * ((mp_word) * tmpx++);
      }
    }

    /* now fix carry for next digit, W[ix+1] */
    W[ix + 1] += W[ix] >> ((mp_word) DIGIT_BIT);
  }

  /* nox fix rest of carries */
  for (++ix; ix <= m->used * 2 + 1; ix++) {
    W[ix] += (W[ix - 1] >> ((mp_word) DIGIT_BIT));
  }

  /* copy out, A = A/b^n
   *
   * The result is A/b^n but instead of converting from an array of mp_word
   * to mp_digit than calling mp_rshd we just copy them in the right
   * order
   */
  for (ix = 0; ix < m->used + 1; ix++) {
    a->dp[ix] = W[ix + m->used] & ((mp_word) MP_MASK);
  }

  /* set the max used */
  a->used = m->used + 1;

  /* zero oldused digits, if the input a was larger than
   * m->used+1 we'll have to clear the digits */
  for (; ix < olduse; ix++) {
    a->dp[ix] = 0;
  }
  mp_clamp (a);

  /* if A >= m then A = A - m */
  if (mp_cmp_mag (a, m) != MP_LT) {
    return s_mp_sub (a, m, a);
  }
  return MP_OKAY;
}
