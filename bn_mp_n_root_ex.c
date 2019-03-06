#include "tommath_private.h"
#ifdef BN_MP_N_ROOT_EX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * SPDX-License-Identifier: Unlicense
 */

/*
 * Find the n'th root of an integer.
 *
 * Result found such that (c)**b <= a and (c+1)**b > a
 *
 */
#if (( (defined LTM_USE_FASTER_VERSIONS) || (defined LTM_USE_FASTER_NTH_ROOT))  )
/* TODO: needs a benchmark script and be put in bncore.c */
static const int NTHROOT_NEWTON_HALLEY_CUTOFF = 100000;

/*
   On a system with Gnu LibC > 4 you can use
   __builtin_clz() or the assembler command BSR
   (Intel) but let me asure you: the function
   floor_log2() will not be the bottleneck here.
*/
static inline int floor_log2(mp_digit value)
{
   int r = 0;
   while ((value >>= 1) != 0) {
      r++;
   }
   return r;
}
/*
  Extra version for int needed because mp_digit is
     a) unsigned
     b) can be any size between 8 and 64 bits
  Two version with the same code, just different
  input types seems silly but all the ways known to
  me than can work around that are either complicated
  or dependend on compiler specifics or are ugly or
  all of the above.
  An example for "all of the above":
  #define FLOOR_ILOG2(T)              \
      int floor_ilog2_##T(T value) {  \
        int r = 0;                    \
        while ((value >>= 1) != 0) {  \
          r++;                        \
        }                             \
        return r;                     \
      }
  FLOOR_ILOG2(int)
  FLOOR_ILOG2(mp_digit)
*/

/*  
    Here, "value" will not be negative, so it is, in theory,
    possible to use the function above by casting "int" to
    "mp_digit" but "mp_digit" can be smaller than "int", much
    smaller. 
 */
static inline int floor_ilog2(int value)
{
   int r = 0;
   while ((value >>= 1) != 0) {
      r++;
   }
   return r;
}
/*
   The cut-off between Newton's method and bisection is at
   about ln(x)/(ln ln (x)) * 1.2.
   Floating point methods are not available, so a rough
   approximation must do.
   By taking the bitcount of the number as floor(log_2(x))
   and, together with ln(x) ~ floor(log2(x)) * log(2)
   implemented as 69/100 *  floor(log2(x)), we can get
   a sufficiently good approximation.
   This snippet assumes "int" is at least 16 bit wide.
   TODO: check if it is possible to use mp_word instead
         which is guaranteed to be at least 16 bit wide
*/
#include <limits.h>
static int recurrence_bisection_cutoff(int value)
{
   int lnx, lnlnx;

   /*
      such small values should have been handled by a nth-root
      implementation with native integers
   */
   if (value < 8) {
      return 1;
   }

   /* ln(x) ~ floor(log2(x)) * log(2) */
   if (value > ((INT_MAX / 69))) {
      /*
         if "value" is so big that a multiplication
         with 69 overflows we can safely spend
         two digits of accuracy for a better sleep.
      */
      lnx = (value / 100) * 69;
   } else {
      lnx = ((69 * value) / 100);
   }
   /* ln ln x */
   lnlnx = floor_ilog2(lnx);
   /* cannot overflow anymore here */
   lnlnx = ((69 * lnlnx) / 100);

   lnx  = lnx / lnlnx;
   /* floor(ln(x)/(ln ln (x))) < floor(fln2(x)/(fln2 fln2 (x))) + 1 for x >= 8 */
   lnx += 1;
   /*  apply twiddle factor */
   /* cannot overflow */
   lnx = ((12 * lnx) / 10);
   return lnx;
}

/*
   Compute log_2(b) bits of a^(1/b) or all of them with a binary search method
*/
static int bisection(mp_int *a, mp_digit b, mp_int *c, int cutoff, int rootsize)
{
   mp_int low, high, mid, midpow;
   int e, comp, i = 0;

   /* force at least one run */
   if (cutoff == 0) {
      cutoff = 1;
   }

   if ((e = mp_init_multi(&low, &high, &mid, &midpow, NULL)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_2expt(&high, rootsize)) != MP_OKAY) {
      goto LTM_ERR_FULL_BIS;
   }
   if ((e = mp_2expt(&low, rootsize - 2)) != MP_OKAY) {
      goto LTM_ERR_FULL_BIS;
   }
   while (mp_cmp(&low, &high) == MP_LT) {
      if (i++ == cutoff) {
         mp_exch(&high, c);
         goto LTM_ERR_FULL_BIS;
      }
      if ((e = mp_add(&low, &high, &mid)) != MP_OKAY) {
         goto LTM_ERR_FULL_BIS;
      }
      if ((e = mp_div_2(&mid, &mid)) != MP_OKAY) {
         goto LTM_ERR_FULL_BIS;
      }
      mp_expt_d(&mid, b, &midpow);
      comp = mp_cmp(&midpow, a);
      if (mp_cmp(&low, &mid) == MP_LT && comp == MP_LT) {
         mp_exch(&low, &mid);
      } else if (mp_cmp(&high, &mid) == MP_GT && comp == MP_GT) {
         mp_exch(&high, &mid);
      } else {
         mp_exch(&mid, c);
         goto LTM_ERR_FULL_BIS;
      }
   }
   if ((e = mp_add_d(&mid, 1, &mid)) != MP_OKAY) {
      goto LTM_ERR_FULL_BIS;
   }
   mp_exch(&mid, c);
LTM_ERR_FULL_BIS:
   mp_clear_multi(&low, &high, &mid, &midpow, NULL);
   return e;
}

static int halley(mp_int *a,mp_digit  b,mp_int *c, int cutoff, int rootsize)
{
   mp_int xi, t1, t2, nom, den, amb, apb;
   int e = MP_OKAY;

   if ((e = mp_init_multi(&xi, &t1, &t2, &nom, &den, &amb, &apb, NULL)) != MP_OKAY) {
      goto LTM_T3;
   }

   if ((e = bisection(a, b, &xi, cutoff, rootsize)) != MP_OKAY) {
      goto LTM_T3;
   }

   if ((e = mp_mul_d(a, b - 1, &amb)) != MP_OKAY) {
      goto LTM_T3;
   }
   if ((e = mp_mul_2(a, &apb)) != MP_OKAY) {
      goto LTM_T3;
   }
   if ((e = mp_add(&amb, &apb, &apb)) != MP_OKAY) {
      goto LTM_T3;
   }
   while (mp_cmp(&xi, &t1) != MP_EQ) {
      if ((e = mp_copy(&xi, &t1)) != MP_OKAY) {
         return e;
      }
      if ((e = mp_expt_d(&xi, b, &t2)) != MP_OKAY) {
         goto LTM_T3;
      }
      if ((e = mp_mul_d(&t2, b - 1, &nom)) != MP_OKAY) {
         goto LTM_T3;
      }
      if ((e = mp_add(&apb, &nom, &nom)) != MP_OKAY) {
         goto LTM_T3;
      }
      if ((e = mp_mul(&xi, &nom, &nom)) != MP_OKAY) {
         goto LTM_T3;
      }
      /* TODO: check for overflow of b+1? */
      if ((e = mp_mul_d(&t2, b + 1, &t2)) != MP_OKAY) {
         goto LTM_T3;
      }
      if ((e = mp_add(&amb, &t2, &den)) != MP_OKAY) {
         goto LTM_T3;
      }
      if ((e = mp_div(&nom, &den, &xi, NULL)) != MP_OKAY) {
         goto LTM_T3;
      }
   }
   mp_exch(&xi, c);
LTM_T3:
   mp_clear_multi(&xi, &t1, &t2, &nom, &den, &amb, &apb, NULL);
   return e;
}

static int newton(mp_int *a, mp_digit b, mp_int *c, int cutoff, int rootsize)
{
   mp_int xi, t1, t2;
   int e = MP_OKAY;

   if ((e = mp_init_multi(&xi, &t1, &t2, NULL)) != MP_OKAY) {
      return e;
   }
   if ((e = bisection(a, b, &t1, cutoff, rootsize)) != MP_OKAY) {
      goto LTM_ERR_NEWTON;
   }
   if ((e = mp_add_d(&t1, 1, &xi)) != MP_OKAY) {
      goto LTM_ERR_NEWTON;
   }
   while (mp_cmp(&t1, &xi) == MP_LT) {
      if ((e = mp_copy(&t1, &xi)) != MP_OKAY) {
         goto LTM_ERR_NEWTON;
      }
      if ((e = mp_mul_d(&xi, b - 1, &t2)) != MP_OKAY) {
         goto LTM_ERR_NEWTON;
      }
      if ((e = mp_expt_d(&xi, b - 1, &t1)) != MP_OKAY) {
         goto LTM_ERR_NEWTON;
      }
      if ((e = mp_div(a, &t1, &t1, NULL)) != MP_OKAY) {
         goto LTM_ERR_NEWTON;
      }
      if ((e = mp_add(&t1, &t2, &t1)) != MP_OKAY) {
         goto LTM_ERR_NEWTON;
      }
      if ((e = mp_div_d(&t1, b, &t1, NULL)) != MP_OKAY) {
         goto LTM_ERR_NEWTON;
      }
   }
   mp_exch(&xi, c);
LTM_ERR_NEWTON:
   mp_clear_multi(&xi, &t1, &t2, NULL);
   return e;
}

#ifdef LTM_USE_SMALL_NTH_ROOT
/*
   Using native types for single limbs is faster by
   several magnitudes.
*/
static mp_word small_pow(mp_word x, mp_digit n)
{
   mp_word r;
   r = (mp_word)1;
   while (n != 0) {
      if (n & 1) {
         r *= x;
      }
      x *= x;
      n >>= 1;
   }
   return r;
}

static mp_word floor_log2_word(mp_word value)
{
   mp_word r = 0;
   while ((value >>= 1) != 0) {
      r++;
   }
   return r;
}

static mp_digit small_nthroot(mp_digit a, mp_digit b)
{
   mp_word x0;
   mp_word xn,x1,A,B;

   /* no checks, assuming caller had done them all */

   A = (mp_word)(a);
   B = (mp_word)(b);

   x0 = (mp_word)(1) << ((floor_log2_word(A) / B) + 1);

   do {
      x1 = x0;
      xn = small_pow(x1,b);
      x0 = (x1 *(((B+1)*A) + ((B-1)*xn))) / ((B-1)*A + (B+1)*xn);
   } while (x1 != x0);

   /*
      Unproven: correction should not be necessary
      if the startvalue is bigger than the root
   */
   xn = small_pow(x0,b);
   if (xn > (mp_word)a) {
      x0--;
      for (;;) {
         xn = small_pow(x0,b);
         if (xn <= (mp_word)a) {
            break;
         }
         x0--;
      }
   } else if (xn < (mp_word)a) {
      x0++;
      for (;;) {
         xn = small_pow(x0,b);
         if (xn >= (mp_word)a) {
            break;
         }
         x0++;
      }
      x0--;
   }
   return (mp_digit)x0;
}
#endif
int mp_n_root_ex(const mp_int *a, mp_digit b, mp_int *c, int fast)
{
   mp_int A;
#ifndef LTM_I_TRUST_THE_RABBIT
   mp_int t1;
   int cmp;
#endif
   int e = MP_OKAY, ilog2, neg, rootsize, cutoff, even_faster;

   /*
    * Checks, balances and shortcuts
    *
    * if b = 0             -> MP_VAL division by zero
    * if b even and a neg. -> MP_VAL non-real result
    * if a = 0 and b > 0   -> 0
    * if a = 0 and b < 0   -> n/a    b is unsigned
    * if a = 1             -> 1
    * if a > 0 and b < 0   -> n/a    b is unsigned
    * if b > log_2(a)      -> 1
    */

   if (b == 0) {
      return MP_VAL;
   }

   if (b == 1) {
      if ((e = mp_copy(a, c)) != MP_OKAY) {
         return e;
      }
      return MP_OKAY;
   }
   if (b == 2) {
      return mp_sqrt(a, c);
   }

   /* TODO: check if an exception for unity is sensible */
   if ((a->used == 1) && (a->dp[0] == 1)) {
      mp_set(c, 1);
      if (a->sign == MP_NEG && ((b & 1) == 0)) {
         c->sign = MP_NEG;
      }
      return MP_OKAY;
   }

   if ((a->sign == MP_NEG) && ((b & 1) == 0)) {
      return MP_VAL;
   }

   /* The type "mp_digit" can be bigger than int */
   if (sizeof(mp_digit) > sizeof(int) &&  b > INT_MAX) {
      /* In that case "b" is bigger than log_2(x), hence floor(x^(1/b)) = 1 */
      mp_set(c, 1);
      c->sign = a->sign;
      return MP_OKAY;
   }

   if (mp_iszero(a)) {
      mp_zero(c);
      return MP_OKAY;
   }

#ifdef LTM_USE_SMALL_NTH_ROOT
   if (a->used == 1) {
      ilog2 = small_nthroot(a->dp[0],  b);
      mp_set(c,ilog2);
      return MP_OKAY;
   }
#endif

   A = *a;
   neg = a->sign;
   A.sign = MP_ZPOS;

   ilog2 = mp_count_bits(a);

   if (ilog2 < (int)(b)) {
      mp_set(c, 1uL);
      c->sign = neg;
      return MP_OKAY;
   }

   rootsize = (ilog2/(int)(b)) + 1;
   cutoff = floor_log2(b);

   /* to avoid warning now and for later extensions */
   if (fast == 0) {
      fast = 1;
   }

   if (fast == 1) {
      even_faster = recurrence_bisection_cutoff(ilog2);
      if (b < (mp_digit)even_faster) {
         if (ilog2 > NTHROOT_NEWTON_HALLEY_CUTOFF) {
            if ((e = halley(&A, b, c, cutoff, rootsize)) != MP_OKAY) {
               return e;
            }
         } else {
            if ((e = newton(&A, b, c, cutoff, rootsize)) != MP_OKAY) {
               return e;
            }
         }
      } else {
         if ((e = bisection(&A, b, c, -1, rootsize)) != MP_OKAY) {
            return e;
         }
      }
   } else {
      if ((e = bisection(&A, b, c, -1, rootsize)) != MP_OKAY) {
         return e;
      }
   }
#ifndef LTM_I_TRUST_THE_RABBIT
   if ((e = mp_init(&t1)) != MP_OKAY) {
      return e;
   }
   cmp = mp_cmp(c, &A);
   if (cmp == MP_GT) {
      if ((e = mp_sub_d(c, 1u, c)) != MP_OKAY) {
         goto LTM_ERR;
      }
      for (;;) {
         if ((e = mp_expt_d(c, b, &t1)) != MP_OKAY) {
            goto LTM_ERR;
         }
         cmp = mp_cmp(&t1, &A);
         if (cmp != MP_GT) {
            break;
         }
         if ((e = mp_sub_d(c, 1u, c)) != MP_OKAY) {
            goto LTM_ERR;
         }
      }
   } else if (cmp == MP_LT) {
      if ((e = mp_add_d(c, 1u, c)) != MP_OKAY) {
         goto LTM_ERR;
      }
      for (;;) {
         if ((e = mp_expt_d(c, b, &t1)) != MP_OKAY) {
            goto LTM_ERR;
         }
         cmp = mp_cmp(&t1, &A);
         if (cmp != MP_LT) {
            break;
         }
         if ((e = mp_add_d(c, 1u, c)) != MP_OKAY) {
            goto LTM_ERR;
         }
      }
      /* Does overshoot in contrast to the other branch above */
      if (cmp != MP_EQ) {
         if ((e = mp_sub_d(c, 1u, c)) != MP_OKAY) {
            goto LTM_ERR;
         }
      }
   }

LTM_ERR:
   mp_clear(&t1);
#endif
   c->sign = neg;
   return e;
}
#else
int mp_n_root_ex(const mp_int *a, mp_digit b, mp_int *c, int fast)
{
   mp_int  t1, t2, t3, a_;
   int   res, cmp;
   int ilog2;

   /* input must be positive if b is even */
   if (((b & 1u) == 0u) && (a->sign == MP_NEG)) {
      return MP_VAL;
   }

   if ((res = mp_init(&t1)) != MP_OKAY) {
      return res;
   }

   if ((res = mp_init(&t2)) != MP_OKAY) {
      goto LBL_T1;
   }

   if ((res = mp_init(&t3)) != MP_OKAY) {
      goto LBL_T2;
   }

   /* if a is negative fudge the sign but keep track */
   a_ = *a;
   a_.sign = MP_ZPOS;

#if 0
   /* t2 = 2 */
   mp_set(&t2, 2uL);
#endif
   ilog2 = mp_count_bits(a);
   if (ilog2 < (int)b) {
      mp_set(c, 1uL);
      c->sign = a->sign;
      res = MP_OKAY;
      goto LBL_T3;
   }

   ilog2 = (int)( ( (mp_digit) ilog2 ) / b );
   if (ilog2 == 0) {
      mp_set(c, 1uL);
      c->sign = a->sign;
      res = MP_OKAY;
      goto LBL_T3;
   }
   /* Start value must be larger than root */
   if ((  res = mp_2expt(&t2,ilog2 + 2)) != MP_OKAY) {
      goto LBL_T3;
   }

   do {
      /* t1 = t2 */
      if ((res = mp_copy(&t2, &t1)) != MP_OKAY) {
         goto LBL_T3;
      }

      /* t2 = t1 - ((t1**b - a) / (b * t1**(b-1))) */

      /* t3 = t1**(b-1) */
      if ((res = mp_expt_d_ex(&t1, b - 1u, &t3, fast)) != MP_OKAY) {
         goto LBL_T3;
      }

      /* numerator */
      /* t2 = t1**b */
      if ((res = mp_mul(&t3, &t1, &t2)) != MP_OKAY) {
         goto LBL_T3;
      }

      /* t2 = t1**b - a */
      if ((res = mp_sub(&t2, &a_, &t2)) != MP_OKAY) {
         goto LBL_T3;
      }

      /* denominator */
      /* t3 = t1**(b-1) * b  */
      if ((res = mp_mul_d(&t3, b, &t3)) != MP_OKAY) {
         goto LBL_T3;
      }

      /* t3 = (t1**b - a)/(b * t1**(b-1)) */
      if ((res = mp_div(&t2, &t3, &t3, NULL)) != MP_OKAY) {
         goto LBL_T3;
      }

      if ((res = mp_sub(&t1, &t3, &t2)) != MP_OKAY) {
         goto LBL_T3;
      }
   }  while (mp_cmp(&t1, &t2) != MP_EQ);

   /* result can be off by a few so check */
   /* Overshoot by one if root is smaller */
   for (;;) {
      if ((res = mp_expt_d_ex(&t1, b, &t2, fast)) != MP_OKAY) {
         goto LBL_T3;
      }
      cmp = mp_cmp(&t2, &a_);
      if (cmp == MP_EQ) {
         res = MP_OKAY;
         goto LBL_T3;
      }
      if (cmp == MP_LT) {
         if ((res = mp_add_d(&t1, 1uL, &t1)) != MP_OKAY) {
            goto LBL_T3;
         }
      } else {
         break;
      }
   }
   /* correct overshoot from above or from recurrence */
   for (;;) {
      if ((res = mp_expt_d_ex(&t1, b, &t2, fast)) != MP_OKAY) {
         goto LBL_T3;
      }
      if (mp_cmp(&t2, &a_) == MP_GT) {
         if ((res = mp_sub_d(&t1, 1uL, &t1)) != MP_OKAY) {
            goto LBL_T3;
         }
      } else {
         break;
      }
   }

   /* set the result */
   mp_exch(&t1, c);

   /* set the sign of the result */
   c->sign = a->sign;

   res = MP_OKAY;

LBL_T3:
   mp_clear(&t3);
LBL_T2:
   mp_clear(&t2);
LBL_T1:
   mp_clear(&t1);
   return res;
}

#endif

#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
