#include "tommath_private.h"
#ifdef BN_MP_N_ROOT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/*
 * Find the n'th root of an integer.
 *
 * Result found such that (c)**b <= a and (c+1)**b > a
 *
 * This algorithm uses Newton's approximation
 * x[i+1] = x[i] - f(x[i])/f'(x[i])
 * which will find the root in log(N) time where
 * each step involves a fair bit.
 */

#ifdef LTM_USE_SMALLER_NTH_ROOT
mp_err mp_n_root(const mp_int *a, mp_digit b, mp_int *c)
{
   mp_int t1, t2, t3, a_;
   mp_ord cmp;
   int ilog2;
   mp_err err;

   /* input must be positive if b is even */
   if (((b & 1u) == 0u) && (a->sign == MP_NEG)) {
      return MP_VAL;
   }

   if ((err = mp_init_multi(&t1, &t2, &t3, NULL)) != MP_OKAY) {
      return err;
   }

   /* if a is negative fudge the sign but keep track */
   a_ = *a;
   a_.sign = MP_ZPOS;

   /* Compute seed: 2^(log_2(n)/b + 2)*/
   ilog2 = mp_count_bits(a);

   /*
      GCC and clang do not understand the sizeof tests and complain,
      icc (the Intel compiler) seems to understand, at least it doesn't complain.
      2 of 3 say these macros are necessary, so there they are.
   */
#if ( !(defined MP_8BIT) && !(defined MP_16BIT) )
   /*
       The type of mp_digit might be larger than an int.
       If "b" is larger than INT_MAX it is also larger than
       log_2(n) because the bit-length of the "n" is measured
       with an int and hence the root is always < 2 (two).
    */
   if (sizeof(mp_digit) >= sizeof(int)) {
      if (b > (mp_digit)(INT_MAX/2)) {
         mp_set(c, 1uL);
         c->sign = a->sign;
         err = MP_OKAY;
         goto LBL_ERR;
      }
   }
#endif
   /* "b" is smaller than INT_MAX, we can cast safely */
   if (ilog2 < (int)b) {
      mp_set(c, 1uL);
      c->sign = a->sign;
      err = MP_OKAY;
      goto LBL_ERR;
   }
   ilog2 =  ilog2 / ((int)b);
   if (ilog2 == 0) {
      mp_set(c, 1uL);
      c->sign = a->sign;
      err = MP_OKAY;
      goto LBL_ERR;
   }
   /* Start value must be larger than root */
   ilog2 += 2;
   if ((err = mp_2expt(&t2,ilog2)) != MP_OKAY) {
      goto LBL_ERR;
   }

   do {
      /* t1 = t2 */
      if ((err = mp_copy(&t2, &t1)) != MP_OKAY) {
         goto LBL_ERR;
      }

      /* t2 = t1 - ((t1**b - a) / (b * t1**(b-1))) */

      /* t3 = t1**(b-1) */
      if ((err = mp_expt_d(&t1, b - 1u, &t3)) != MP_OKAY) {
         goto LBL_ERR;
      }
      /* numerator */
      /* t2 = t1**b */
      if ((err = mp_mul(&t3, &t1, &t2)) != MP_OKAY) {
         goto LBL_ERR;
      }

      /* t2 = t1**b - a */
      if ((err = mp_sub(&t2, &a_, &t2)) != MP_OKAY) {
         goto LBL_ERR;
      }

      /* denominator */
      /* t3 = t1**(b-1) * b  */
      if ((err = mp_mul_d(&t3, b, &t3)) != MP_OKAY) {
         goto LBL_ERR;
      }

      /* t3 = (t1**b - a)/(b * t1**(b-1)) */
      if ((err = mp_div(&t2, &t3, &t3, NULL)) != MP_OKAY) {
         goto LBL_ERR;
      }

      if ((err = mp_sub(&t1, &t3, &t2)) != MP_OKAY) {
         goto LBL_ERR;
      }
      /*
          Number of rounds is at most log_2(root). If it is more it
          got stuck, so break out of the loop and do the rest manually.
       */
      if (ilog2-- == 0) {
         break;
      }
   }  while (mp_cmp(&t1, &t2) != MP_EQ);

   /* result can be off by a few so check */
   /* Loop beneath can overshoot by one if found root is smaller than actual root */
   for (;;) {
      if ((err = mp_expt_d(&t1, b, &t2)) != MP_OKAY) {
         goto LBL_ERR;
      }
      cmp = mp_cmp(&t2, &a_);
      if (cmp == MP_EQ) {
         err = MP_OKAY;
         goto LBL_ERR;
      }
      if (cmp == MP_LT) {
         if ((err = mp_add_d(&t1, 1uL, &t1)) != MP_OKAY) {
            goto LBL_ERR;
         }
      } else {
         break;
      }
   }
   /* correct overshoot from above or from recurrence */
   for (;;) {
      if ((err = mp_expt_d(&t1, b, &t2)) != MP_OKAY) {
         goto LBL_ERR;
      }
      if (mp_cmp(&t2, &a_) == MP_GT) {
         if ((err = mp_sub_d(&t1, 1uL, &t1)) != MP_OKAY) {
            goto LBL_ERR;
         }
      } else {
         break;
      }
   }

   /* set the result */
   mp_exch(&t1, c);

   /* set the sign of the result */
   c->sign = a->sign;

   err = MP_OKAY;

LBL_ERR:
   mp_clear_multi(&t1, &t2, &t3, NULL);
   return err;
}

#else /* LTM_USE_SMALLER_NTH_ROOT */

/*
   On a system with Gnu LibC > 4 you can use
   __builtin_clz() or the assembler command BSR
   (Intel) but let me assure you: the function
   s_floor_log2() will not be the bottleneck here.
*/
static int s_floor_log2(mp_digit value)
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
  or dependent on compiler specifics or are ugly or
  all of the above.
  An example for "all of the above":
  #define FLOOR_ILOG2(T)              \
      int s_floor_ilog2_##T(T value) {  \
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
static int s_floor_ilog2(int value)
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
static int s_recurrence_bisection_cutoff(int value)
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
   lnlnx = s_floor_ilog2(lnx);
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
static mp_err s_bisection(mp_int *a, mp_digit b, mp_int *c, int cutoff, int rootsize)
{
   mp_int low, high, mid, midpow;
   mp_err err;
   int comp, i = 0;

   /* force at least one run */
   if (cutoff == 0) {
      cutoff = 1;
   }

   if ((err = mp_init_multi(&low, &high, &mid, &midpow, NULL)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_2expt(&high, rootsize)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((err = mp_2expt(&low, rootsize - 2)) != MP_OKAY) {
      goto LTM_ERR;
   }
   while (mp_cmp(&low, &high) == MP_LT) {
      if (i++ == cutoff) {
         mp_exch(&high, c);
         goto LTM_ERR;
      }
      if ((err = mp_add(&low, &high, &mid)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((err = mp_div_2(&mid, &mid)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((err = mp_expt_d(&mid, b, &midpow)) != MP_OKAY) {
         goto LTM_ERR;
      }
      comp = mp_cmp(&midpow, a);
      if (mp_cmp(&low, &mid) == MP_LT && comp == MP_LT) {
         mp_exch(&low, &mid);
      } else if (mp_cmp(&high, &mid) == MP_GT && comp == MP_GT) {
         mp_exch(&high, &mid);
      } else {
         mp_exch(&mid, c);
         goto LTM_ERR;
      }
   }
   if ((err = mp_add_d(&mid, 1, &mid)) != MP_OKAY) {
      goto LTM_ERR;
   }
   mp_exch(&mid, c);
LTM_ERR:
   mp_clear_multi(&low, &high, &mid, &midpow, NULL);
   return err;
}

static mp_err s_newton(mp_int *a, mp_digit b, mp_int *c, int cutoff, int rootsize)
{
   mp_int xi, t1, t2;
   mp_err err = MP_OKAY;

   if ((err = mp_init_multi(&xi, &t1, &t2, NULL)) != MP_OKAY) {
      return err;
   }
   if ((err = s_bisection(a, b, &t1, cutoff, rootsize)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((err = mp_add_d(&t1, 1, &xi)) != MP_OKAY) {
      goto LTM_ERR;
   }
   while (mp_cmp(&t1, &xi) == MP_LT) {
      if ((rootsize--) == 0) {
         break;
      }
      if ((err = mp_copy(&t1, &xi)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((err = mp_mul_d(&xi, b - 1, &t2)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((err = mp_expt_d(&xi, b - 1, &t1)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((err = mp_div(a, &t1, &t1, NULL)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((err = mp_add(&t1, &t2, &t1)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if ((err = mp_div_d(&t1, b, &t1, NULL)) != MP_OKAY) {
         goto LTM_ERR;
      }
   }
   mp_exch(&xi, c);
LTM_ERR:
   mp_clear_multi(&xi, &t1, &t2, NULL);
   return err;
}

mp_err mp_n_root(const mp_int *a, mp_digit b, mp_int *c)
{
   mp_int A;
   mp_int t1;
   int cmp;
   mp_err err = MP_OKAY;
   int ilog2, rootsize, cutoff, even_faster;
   mp_sign neg;

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
      if ((err = mp_copy(a, c)) != MP_OKAY) {
         return err;
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
#if ( !(defined MP_8BIT) && !(defined MP_16BIT) )
   /* The type "mp_digit" can be bigger than int */
   if (sizeof(mp_digit) > sizeof(int) &&  b > INT_MAX) {
      /* In that case "b" is bigger than log_2(x), hence floor(x^(1/b)) = 1 */
      mp_set(c, 1);
      c->sign = a->sign;
      return MP_OKAY;
   }
#endif
   if (mp_iszero(a)) {
      mp_zero(c);
      return MP_OKAY;
   }

#ifdef LTM_USE_SMALL_NTH_ROOT
   if (a->used == 1) {
      ilog2 = s_small_nthroot(a->dp[0],  b);
      mp_set(c,ilog2);
      return MP_OKAY;
   }
#endif
   if ((err = mp_init(&A)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_copy(a, &A)) != MP_OKAY) {
      goto LTM_ERR_2;
   }
   neg = a->sign;
   A.sign = MP_ZPOS;

   ilog2 = mp_count_bits(a);

   if (ilog2 < (int)(b)) {
      mp_set(c, 1uL);
      c->sign = neg;
      goto LTM_ERR_2;
   }

   rootsize = (ilog2/(int)(b)) + 1;
   cutoff = s_floor_log2(b);

   even_faster = s_recurrence_bisection_cutoff(ilog2);
   if (b < (mp_digit)even_faster) {
      if ((err = s_newton(&A, b, c, cutoff, rootsize)) != MP_OKAY) {
         goto LTM_ERR_2;
      }
   } else {
      if ((err = s_bisection(&A, b, c, -1, rootsize)) != MP_OKAY) {
         goto LTM_ERR_2;
      }
   }

   if ((err = mp_init(&t1)) != MP_OKAY) {
      goto LTM_ERR_2;
   }
   if ((err = mp_expt_d(c, b, &t1)) != MP_OKAY) {
      goto LTM_ERR_1;
   }
   cmp = mp_cmp(&t1, &A);
   if (cmp == MP_GT) {
      if ((err = mp_sub_d(c, 1u, c)) != MP_OKAY) {
         goto LTM_ERR_1;
      }
      for (;;) {
         if ((err = mp_expt_d(c, b, &t1)) != MP_OKAY) {
            goto LTM_ERR_1;
         }
         cmp = mp_cmp(&t1, &A);
         if (cmp != MP_GT) {
            break;
         }
         if ((err = mp_sub_d(c, 1u, c)) != MP_OKAY) {
            goto LTM_ERR_1;
         }
      }
   } else if (cmp == MP_LT) {
      if ((err = mp_add_d(c, 1u, c)) != MP_OKAY) {
         goto LTM_ERR_1;
      }
      for (;;) {
         if ((err = mp_expt_d(c, b, &t1)) != MP_OKAY) {
            goto LTM_ERR_1;
         }
         cmp = mp_cmp(&t1, &A);
         if (cmp != MP_LT) {
            break;
         }
         if ((err = mp_add_d(c, 1u, c)) != MP_OKAY) {
            goto LTM_ERR_1;
         }
      }
      /* Does overshoot in contrast to the other branch above */
      if (cmp != MP_EQ) {
         if ((err = mp_sub_d(c, 1u, c)) != MP_OKAY) {
            goto LTM_ERR_1;
         }
      }
   }

LTM_ERR_1:
   mp_clear(&t1);
LTM_ERR_2:
   mp_clear(&A);
   c->sign = a->sign;
   return err;
}
#endif

#endif
