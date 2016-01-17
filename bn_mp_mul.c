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

extern int some_flag;

int mp_mul(mp_int *a, mp_int *b, mp_int *c)
{
   int     res, neg;
#ifdef BN_MP_BALANCE_MUL_C
   int len_b, len_a;
#endif
   neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;
#ifdef BN_MP_BALANCE_MUL_C
   len_a = a->used;
   len_b = b->used;

   if (len_a == len_b){
      goto GO_ON;
   }

   // FFT has its own
   if (MIN(len_a, len_b) >= FFT_MUL_CUTOFF)
   {
      goto GO_ON;
   }

   /*
    * Size check for linear balance
    * Check sizes. The smaller one needs to be larger than the Karatsuba cut-off.
    * The bigger one needs to be at leat about one KARATSUBA_MUL_CUTOFF bigger to make some
    * sense, but it depends on architecture, OS and position of the planets, so YMMV.
    */
   if (MIN(len_a, len_b) < KARATSUBA_MUL_CUTOFF
       || (MAX(len_a, len_b)) / 2 < KARATSUBA_MUL_CUTOFF) {
      goto GO_ON;
   }
   /*
    * Not much effect was observed below a ratio of 1:2, but YMMV.
    */
   if( MAX(len_a, len_b) /  MIN(len_a, len_b) < 2 ){
      goto GO_ON;
   }

   //res = mp_balance_mul(a,b,c);
   res = mp_balance_recursive(a,b,c);
   goto END;

GO_ON:
#endif

#if defined  (MP_28BIT) || defined (MP_64BIT)
/* use FFT? */
#ifdef BN_MP_FFT_MUL_C

/*
   FFT has an upper limit caused by rounding erors. After one round of
   Toom-Cook it can cut in again.
 */
if (MIN(a->used, b->used) >= FFT_MUL_CUTOFF &&
    MAX(a->used, b->used) <= FFT_UPPER_LIMIT)
{

   res = mp_fft_mul(a, b, c);
} else

#endif
#endif

#ifdef BN_MP_TOOM_COOK_5_MUL_C
   if (MIN(a->used, b->used) >= TOOM_COOK_5_MUL_CO)
   {
      res = mp_toom_cook_5_mul(a, b, c);
   } else
#endif

#ifdef BN_MP_TOOM_COOK_4_MUL_C
      if (MIN(a->used, b->used) >= TOOM_COOK_4_MUL_CO)
      {
         res = mp_toom_cook_4_mul(a, b, c);
      } else
#endif
         /* use Toom-Cook? */
#ifdef BN_MP_TOOM_MUL_C
         if (MIN(a->used, b->used) >= TOOM_MUL_CUTOFF)
         {
            res = mp_toom_mul(a, b, c);
         } else
#endif
#ifdef BN_MP_KARATSUBA_MUL_C
            /* use Karatsuba? */
            if (MIN(a->used, b->used) >= KARATSUBA_MUL_CUTOFF)
            {
               res = mp_karatsuba_mul(a, b, c);
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
                   MIN(a->used, b->used) <=
                   (1 << ((CHAR_BIT * sizeof(mp_word)) - (2 * DIGIT_BIT)))) {
                  res = fast_s_mp_mul_digs(a, b, c, digs);
               } else
#endif
#ifdef BN_S_MP_MUL_DIGS_C
                  res = s_mp_mul(a, b, c);  /* uses s_mp_mul_digs */
#else
                  res = MP_VAL;
#endif

            }
END:
  c->sign = (c->used > 0) ? neg : MP_ZPOS;
  return res;
}
#endif

/* $Source$ */
/* $Revision: 0.41 $ */
/* $Date: 2007-04-18 09:58:18 +0000 $ */
