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
 * This algorithm uses Halley's approximation (Compact form from 'Obsieger, Boris.
 * "Numerical methods II-Roots and Equation Systems." university-books.eu, 2013)
 *
 *              (x_i * ( (n+1) * A + (n-1) * x_i^n ))
 *  x_(i+1) =  ---------------------------------------
 *                    ((n-1)*A + (n+1)*x_i^n)
 *
 * after some rounds of bisection to get a decent seed (x_0) for that recursion.
 */

/*
 * These helper function can be optimized, quite heavily even, depending on CPU.
 * The current implementation is in pure C and does not assume anything but a
 * standard-compliant (ISO/IEC 9899:2011 at time of coding) C-compiler.
 */
static mp_word word_floor_log2(mp_word value)
{
   mp_word r = 0;
   if (value <= 0) {
      return MP_VAL;
   }
   while ((value >>= 1) != 0) {
      r++;
   }
   return r;
}


static int small_is_power_2(mp_digit x)
{
   int iter = 0;
   while (((x & 1) == 0) && x > 1) {
      iter++;
      x >>= 1;
   }
   if (x == 1) {
      return MP_YES;
   } else {
      return MP_NO;
   }
}

static int is_power_2(mp_int *a)
{
   int length = a->used;
   int i;
   for (i=0; i<length -1; i++) {
      if (a->dp[i] != (mp_digit)(0)) {
         return MP_NO;
      }
   }
   return small_is_power_2(a->dp[length - 1]);
}

static int nthroot_halley_bisection(mp_int *a, mp_digit b, mp_int *c, int ilog2, int cutoff)
{
   mp_int A_1, A_2, X_1, X_2, mid, midpow, t_1, t_2;
   int e, cmp, i, cl2;
   //TODO: compute max. number of Halley recursion for this safe-guard more precisely
   int iter = ilog2;
   mp_digit limit;

   e = MP_OKAY;

   if ((e = mp_init(&X_1)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_init(&X_2)) != MP_OKAY) {
      goto LBL_T1;
   }

   /* first limit for bisection (or seed for Halley's approximation) */
   if ((e = mp_2expt(&X_2, ilog2)) != MP_OKAY) {
      goto LBL_T2;
   }

   // The overhead of the bisection part is not very large but adds
   // significantly to the evaluation of the smallest indices.

   // Empiricaly evaluated curve (with a smaller 64bit AMD CPU) but
   // a small constant like e.g.: 10 (ten) would do, too.
   //TODO: do the proper math instead
   cl2 = a->used * MP_DIGIT_BIT;
   limit = (word_floor_log2(cl2)*3)/2;
   i = 0;
   if (b > limit) {
      if ((e = mp_init_multi(&mid, &midpow, NULL)) != MP_OKAY) {
         goto LBL_T2;
      }
      /* Second limit for bisection */
      if ((e = mp_2expt(&X_1, ilog2 - 2)) != MP_OKAY) {
         goto LBL_T3a;
      }
      /* Actual bisection part, X_2 = high, X_1 = low */
      while (mp_cmp(&X_1, &X_2) == MP_LT) {
         /* We do not want all, just some rounds */
         if (i++ == cutoff) {
            break;
         }
         if ((e = mp_add(&X_1, &X_2, &mid)) != MP_OKAY) {
            goto LBL_T3a;
         }
         if ((e = mp_div_2(&mid, &mid)) != MP_OKAY) {
            goto LBL_T3a;
         }
         if ((e = mp_expt_d(&mid, b, &midpow)) != MP_OKAY) {
            goto LBL_T3a;
         }
         cmp = mp_cmp(&midpow, a);
         if (mp_cmp(&X_1, &mid) == MP_LT && cmp == MP_LT) {
            mp_exch(&X_1, &mid);
         } else if (mp_cmp(&X_2, &mid) == MP_GT && cmp == MP_GT) {
            mp_exch(&X_2, &mid);
         } else {
            mp_exch(&mid, c);
            goto LBL_T3a;
         }
      }
      /* The final result of the bisection as implemented is of-by-one */
      if ((e = mp_add_d(&mid, 1, &mid)) != MP_OKAY) {
         goto LBL_T3a;
      }
      mp_exch(&mid, &X_2);
      /* "midpow" has a lot of memory allocated */
      mp_clear(&midpow);
      /* Flag to avoid double free() at the end */
      //TODO: get rid of flag
      i = -1;

      //TODO: for large indices bisection finds a solution in <= cutoff rounds, so
      //      a check for it here would avoid a full run of Halley's approximation
   } else {
     /* Start value for Newton-Raphson would be
     if ((e = mp_2expt(&X_2, ilog2)) != MP_OKAY) {
         goto LBL_T2;
      }
     */
     /* Arithmetic mean between the two powers-of-two that bracket the actual root */
     if ((e = mp_2expt(&X_1, ilog2 - 1)) != MP_OKAY) {
        goto LBL_T2;
     }
     if ((e = mp_add(&X_1, &X_2, &X_2)) != MP_OKAY) {
        goto LBL_T2;
     }
     if ((e = mp_div_2(&X_2, &X_2)) != MP_OKAY) {
        goto LBL_T2;
     }
   }

   /* Halley's approximation */
   if ((e = mp_init_multi(&A_1, &A_2, &t_1, &t_2, NULL)) != MP_OKAY) {
      goto LBL_T3a;
   }


   if ((e = mp_mul_d(a, b - 1, &A_1)) != MP_OKAY) {
      goto LBL_T3;
   }
   if ((e = mp_mul_2d(a, 1, &t_1)) != MP_OKAY) {
      goto LBL_T3;
   }
   if ((e = mp_add(&A_1, &t_1, &A_2)) != MP_OKAY) {
      goto LBL_T3;
   }

   do {
      if ((e = mp_copy(&X_2, &X_1)) != MP_OKAY) {
         goto LBL_T3;
      }
      if ((e = mp_expt_d(&X_2, b, &X_2)) != MP_OKAY) {
         goto LBL_T3;
      }
      //if(mp_cmp(&X_2, a) == MP_EQ) break; // perfect power (useful?)
      if ((e = mp_mul_d(&X_2, b - 1, &t_1)) != MP_OKAY) {
         goto LBL_T3;
      }
      if ((e = mp_mul_2d(&X_2, 1, &t_2)) != MP_OKAY) {
         goto LBL_T3;
      }
      if ((e = mp_add(&t_1, &t_2, &t_2)) != MP_OKAY) {
         goto LBL_T3;
      }

      if ((e = mp_add(&A_2, &t_1, &t_1)) != MP_OKAY) {
         goto LBL_T3;
      }
      if ((e = mp_add(&A_1, &t_2, &t_2)) != MP_OKAY) {
         goto LBL_T3;
      }

      if ((e = mp_mul(&t_1, &X_1, &t_1)) != MP_OKAY) {
         goto LBL_T3;
      }

      if ((e = mp_div(&t_1, &t_2, &X_2, NULL)) != MP_OKAY) {
         goto LBL_T3;
      }
      iter--;
   } while (mp_cmp(&X_1, &X_2) != MP_EQ && iter >= 0);

   if (iter == 0) {
#ifdef LIBTOMMATH_DEBUG_VERBOSE
      fprintf(stderr,"Too many iterations in Halley's recursion\n");
#endif
      //TODO: add error MP_ITER to tommath.h?
      e = MP_RANGE;
      goto LBL_T3;     
   }

/*
   First order Newton-Raphson would be:

   do{
      if ((e = mp_copy(&X_2, &X_1)) != MP_OKAY) {
         goto LBL_T3;
      }
      mp_mul_d(&X_1, b-1, &t_1);
      mp_expt_d(&X_1,b-1,&t_2);
      mp_div(a,&t_2,&t_2,NULL);
      mp_add(&t_1,&t_2,&t_1);
      mp_div_d(&t_1,b,&X_2,NULL);
   }while(mp_cmp(&X_2, &X_1) == MP_LT);

   This is definitely unstable and needs a safe-guard for loops, too
*/    

   /* Result may be of by a small amount because of accumulated rounding errors */
   if ((e = mp_expt_d(&X_1, b, &X_2)) != MP_OKAY) {
      goto LBL_T3;
   }
   cmp = mp_cmp(&X_2, a);
   if (cmp == MP_GT) {
      if ((e = mp_sub_d(&X_1, 1, &X_1)) != MP_OKAY) {
         goto LBL_T3;
      }
      for (;;) {
         if ((e = mp_expt_d(&X_1, b, &X_2)) != MP_OKAY) {
            goto LBL_T3;
         }
         cmp = mp_cmp(&X_2, a);
         if (cmp != MP_GT) {
            break;
         }
         if ((e = mp_sub_d(&X_1, 1, &X_1)) != MP_OKAY) {
            goto LBL_T3;
         }
      }
   } else if (cmp == MP_LT) {
      if ((e = mp_add_d(&X_1, 1, &X_1)) != MP_OKAY) {
         goto LBL_T3;
      }
      for (;;) {
         if ((e = mp_expt_d(&X_1, b, &X_2)) != MP_OKAY) {
            goto LBL_T3;
         }
         cmp = mp_cmp(&X_2, a);
         if (cmp != MP_LT) {
            break;
         }
         if ((e = mp_add_d(&X_1, 1, &X_1)) != MP_OKAY) {
            goto LBL_T3;
         }
      }
      // Does overshoot in contrast to the other branch above
      if (cmp != MP_EQ) {
         if ((e = mp_sub_d(&X_1, 1, &X_1)) != MP_OKAY) {
            goto LBL_T3;
         }
      }
   }
   // else {
      // nothing to do for a perfect power (cmp == MP_EQ), obviously
   // }
   mp_exch(&X_1, c);

LBL_T3:
   mp_clear_multi(&A_1, &A_2, &t_1, &t_2, NULL);
LBL_T3a:
   if (b > limit ) {
      if (i < 0){
         mp_clear(&mid); 
      }
      else {
         mp_clear_multi(&mid, &midpow, NULL);
      }
   }
LBL_T2:
   mp_clear(&X_2);
LBL_T1:
   mp_clear(&X_1);
   return e;
}


int mp_n_root(mp_int *a, mp_digit b, mp_int *c)
{
   int e, ilog2, neg;
   mp_digit cutoff;

   /*
    * if b = 0             -> MP_VAL
    * if b even and a neg. -> MP_VAL
    * if a = 0 and b > 0   -> 0
    * if a = 0 and b < 0   -> MP_VAL;  b < 0 not used
    * if a = 1             -> 1
    * if a > 0 and b < 0   -> 0;       b < 0 not used
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

   if (((b & 1) == 0) && (a->sign == MP_NEG)) {
      return MP_VAL;
   }
   if ((a->used == 1) && (a->dp[0] == 1)) {
      mp_set(c, 1);
      /* (-1)^(1/n) with n>0 and n odd */
      if (a->sign == MP_NEG) {
         c->sign = MP_NEG;
      }
      return MP_OKAY;
   }
   if (mp_iszero(a)) {
      mp_zero(c);
      return MP_OKAY;
   }

   neg = a->sign;
   a->sign = MP_ZPOS;

   ilog2 = mp_count_bits(a);

   if (is_power_2(a) == MP_YES  && ilog2 == b) {
      a->sign = neg;
      mp_set(c, (mp_digit)2);
      c->sign = neg;
      return MP_OKAY;
   }

   if (ilog2 < (int)(b)) {
      a->sign = neg;
      mp_set(c, 1);
      c->sign = neg;
      return MP_OKAY;
   }

   /*
    * Computation of the number of rounds of bisection needed for a good seed based on
    *
    * BACH, Eric; SORENSON, Jonathan. Sieve algorithms for perfect power testing. Algo-
    * rithmica, 1993, 9. Jg., Nr. 4, S. 313-328. (vid.: after the proof of theorem 3.1.)
    *
    * The actual sieving is not implemented here.
    */

   /* rough but sufficient approximation of log(log(a))/log_2(log_2(a)) */
   cutoff = (word_floor_log2(ilog2)*2)/3;

   /*  first approximation of nth-root(x) */
   ilog2 = (int)(((mp_digit) ilog2) / b) + 1;

   if ((e = nthroot_halley_bisection(a, b, c, ilog2, cutoff)) != MP_OKAY) {
      a->sign = neg;
      return e;
   }

   a->sign = neg;
   c->sign = neg;
   return MP_OKAY;
}
#endif

/* $Source$ */
/* $Revision$ */
/* $Date$ */
