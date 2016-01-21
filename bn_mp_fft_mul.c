#include <tommath.h>
#ifdef BN_MP_FFT_MUL_C
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

/* Multiplication with FHT convolution */

#if defined  (MP_28BIT) || defined (MP_64BIT)


int mp_fft_mul(mp_int *a, mp_int *b, mp_int *c)
{
   double *fa, *fb;
   fa = NULL;
   fb = NULL;
   int length, e;
   mp_int r;

   if (mp_iszero(a) == MP_YES || mp_iszero(b) == MP_YES) {
      return MP_OKAY;
   }
   if (a->used == 1) {
      if (a->dp[0] == 1) {
         if ((e = mp_copy(b,c)) != MP_OKAY) {
            return e;
         }
      }
      return MP_OKAY;
   }
   if (b->used == 1) {
      if (b->dp[0] == 1) {
         if ((e = mp_copy(a,c)) != MP_OKAY) {
            return e;
         }
      }
      return MP_OKAY;
   }

   /* rounds the edges of the stair a bit (good for equal sized multipliers) */
   /*  il2 = ilog2((unsigned int)MAX(a->used,b->used));
     extra = (il2<12)?powtwos[il2-2]:powtwos[8];
     if( a->used < powtwos[il2]+extra ){
       return mp_toom_cook_5_mul(a,b,c);
     }
   */
   if ((e = mp_dp_to_fft(a, &fa, b, &fb, &length)) != MP_OKAY) {
      if (fa != NULL) free(fa);
      if (fb != NULL) free(fb);
      return e;
   }
   if ((e = mp_fft(fa, fb, length)) != MP_OKAY) {
      if (fa != NULL) free(fa);
      if (fb != NULL) free(fb);
      return e;
   }
   if( (e = mp_init(&r) ) != MP_OKAY){
      return e;
   }
   if ((e = mp_fft_to_dp(fb, &r, length)) != MP_OKAY) {
      if (fa != NULL) free(fa);
      if (fb != NULL) free(fb);
      mp_clear(&r);
      return e;
   }
   free(fa);
   free(fb);
   mp_exch(&r,c);
   mp_clear(&r);
   return MP_OKAY;
}

#else
#error only for 28 bit digits for now
#endif

#endif
