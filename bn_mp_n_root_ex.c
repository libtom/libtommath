#include "tommath_private.h"
#ifdef BN_MP_N_ROOT_EX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* find the n'th root of an integer
 *
 * Result found such that (c)**b <= a and (c+1)**b > a
 *
 * This algorithm uses Newton's approximation
 * x[i+1] = x[i] - f(x[i])/f'(x[i])
 * which will find the root in log(N) time where
 * each step involves a fair bit.
 */
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

   /* Compute seed: 2^(log_2(n)/b + 2)*/
   ilog2 = mp_count_bits(a);

   /*
      GCC and clang do not understand the sizeof(bla) tests and complain,
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
         res = MP_OKAY;
         goto LBL_T3;
      }
   }
#endif
   /* "b" is smaller than INT_MAX, we can cast safely */
   if (ilog2 < (int)b) {
      mp_set(c, 1uL);
      c->sign = a->sign;
      res = MP_OKAY;
      goto LBL_T3;
   }
   ilog2 =  ilog2 / ((int)b);
   if (ilog2 == 0) {
      mp_set(c, 1uL);
      c->sign = a->sign;
      res = MP_OKAY;
      goto LBL_T3;
   }
   /* Start value must be larger than root */
   ilog2 += 2;
   if ((res = mp_2expt(&t2,ilog2)) != MP_OKAY) {
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
