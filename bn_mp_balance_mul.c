#include <tommath.h>
#ifdef BN_MP_BALANCE_MUL_C
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

/* c = |a| * |b| using balancing multiplication.
 * If |a| is much less than |b|, 
 * we firstly split b into chunks such that length of each one is 
 * roughly equal to that of |a|.
 */
int mp_balance_mul (mp_int * a, mp_int * b, mp_int * c)
{
  /* the following algorithm is taken from 
   * Ruby core; namely, function 'bigmul1_balance'
   * from 'bignum.c'
   */
  mp_int t1, t2;
  long i, an, bn, r, n;
  int res,  min, max;
  int err = MP_MEM;

  mp_digit *bds, *t1ds;

  an = a->used;
  bn = b->used;
  if ((res = mp_grow(c, an + bn)) != MP_OKAY) {
    return res;
  }

  if (mp_init_size(&t1, an) != MP_OKAY) {
    goto ERR;
  }

  bds = b->dp;
  t1ds = t1.dp;

  n = 0;

  mp_int x;

  c->used = an + bn;
  while (bn > 0) {
    r = MIN(an, bn);
    for (i = 0; i < r; ++i)
      t1ds[i] = bds[n + i];
    t1.used = r;

    mp_init_size(&t2, an + r);
    mp_mul(a, &t1, &t2);

    if (t2.used > c->used - n) {
      min = c->used - n; max = t2.used;
      x.used = t2.used; x.dp = t2.dp;
    } else {
      min = t2.used; max = c->used - n;
      x.used = c->used - n; x.dp = c->dp + n;
    }

    register mp_digit u, *tmpx, *tmpt2, *tmpcn;
    register int j;
    tmpx = tmpcn = x.dp; tmpt2 = t2.dp;
    u = 0;
    for (j = 0; j < min; j++) {
      *tmpcn = *tmpx++ + *tmpt2++ + u;
      u = *tmpcn >> ((mp_digit)DIGIT_BIT);
      *tmpcn++ &= MP_MASK;
    }
    if (min != max) {
      for (; j < max; j++) {
        *tmpcn = x.dp[j] + u;
        u = *tmpcn >> ((mp_digit)DIGIT_BIT);
        *tmpcn++ &= MP_MASK;
      }
    }
    *tmpcn++ = u;
    
    bn -= r;
    n += r;
  }
  mp_clamp(c);
  return MP_OKAY;
ERR:
  return err;
}
#endif

/* $Source$ */
/* $Revision$ */
/* $Date$ */
