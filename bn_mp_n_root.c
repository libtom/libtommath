#include <tommath.h>
#ifdef BN_MP_N_ROOT_C
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

/* find the n'th root of an integer 
 *
 * Result found such that (c)**b <= a and (c+1)**b > a 
 *
 * This algorithm uses Newton's approximation 
 * x[i+1] = x[i] - f(x[i])/f'(x[i]) 
 * which will find the root in log(N) time where 
 * each step involves a fair bit.
 */
int mp_n_root (mp_int * a, mp_digit b, mp_int * c)
{
  mp_int  t1, t2, t3;
  int     res, neg, ilog2;

  /* input must be positive if b is even */
  if ((b & 1) == 0 && a->sign == MP_NEG) {
    return MP_VAL;
  }

  if ((res = mp_init (&t1)) != MP_OKAY) {
    return res;
  }

  if ((res = mp_init (&t2)) != MP_OKAY) {
    goto LBL_T1;
  }

  if ((res = mp_init (&t3)) != MP_OKAY) {
    goto LBL_T2;
  }

  /* if a is negative fudge the sign but keep track */
  neg     = a->sign;
  a->sign = MP_ZPOS;

  /* Set initial value:
   *     n-th root of r = exp(log_e(r) / n)
   * That can be done with every base for the logarithm, so with base 2 and 
   * integer logarithm:
   *     n-th root of r < 2^( floor( ceil( log_2(r) ) / n) +1 )
   */
  ilog2 = mp_count_bits(a);
  ilog2 = (int)( ( (mp_digit) ilog2 ) / b );
  if ((  res = mp_2expt(&t2,ilog2+1)) != MP_OKAY) {
    goto LBL_T2;
  }

  do {
    /* t1 = t2 */
    if ((res = mp_copy (&t2, &t1)) != MP_OKAY) {
      goto LBL_T3;
    }

    /* t2 = t1 - ((t1**b - a) / (b * t1**(b-1))) */
    
    /* t3 = t1**(b-1) */
    if ((res = mp_expt_d (&t1, b - 1, &t3)) != MP_OKAY) {   
      goto LBL_T3;
    }

    /* numerator */
    /* t2 = t1**b */
    if ((res = mp_mul (&t3, &t1, &t2)) != MP_OKAY) {    
      goto LBL_T3;
    }

    /* t2 = t1**b - a */
    if ((res = mp_sub (&t2, a, &t2)) != MP_OKAY) {  
      goto LBL_T3;
    }

    /* denominator */
    /* t3 = t1**(b-1) * b  */
    if ((res = mp_mul_d (&t3, b, &t3)) != MP_OKAY) {    
      goto LBL_T3;
    }

    /* t3 = (t1**b - a)/(b * t1**(b-1)) */
    if ((res = mp_div (&t2, &t3, &t3, NULL)) != MP_OKAY) {  
      goto LBL_T3;
    }

    if ((res = mp_sub (&t1, &t3, &t2)) != MP_OKAY) {
      goto LBL_T3;
    }
  }  while (mp_cmp (&t1, &t2) != MP_EQ);

  /* 
     The value is most probable 1 (one) digit too large, so to save one
     exponentiation subtract 1 (one) in advance.
     TODO: check if that is always the case, as the author of the patch
           conjectures.
  */
  if ((res = mp_sub_d (&t1, 1, &t1)) != MP_OKAY) {
        goto LBL_T3;
  }
  /* result can be off by a few so check */
  for (;;) {
    if ((res = mp_expt_d (&t1, b, &t2)) != MP_OKAY) {
      goto LBL_T3;
    }

    if (mp_cmp (&t2, a) == MP_GT) {
      if ((res = mp_sub_d (&t1, 1, &t1)) != MP_OKAY) {
         goto LBL_T3;
      }
    } else {
      break;
    }
  }

  /* reset the sign of a first */
  a->sign = neg;

  /* set the result */
  mp_exch (&t1, c);

  /* set the sign of the result */
  c->sign = neg;

  res = MP_OKAY;

LBL_T3:mp_clear (&t3);
LBL_T2:mp_clear (&t2);
LBL_T1:mp_clear (&t1);
  return res;
}
#endif

/* $Source$ */
/* $Revision$ */
/* $Date$ */
