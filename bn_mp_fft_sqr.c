#include <tommath.h>
#ifdef BN_MP_FFT_SQR_C
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

#if defined ( MP_28BIT) || defined (MP_64BIT)

int mp_fft_sqr(mp_int *a,mp_int *c)
{
   double *fa, *fb;
   fa = NULL;
   fb = NULL;
   int length, e;
   if (mp_iszero(a) == MP_YES) {
      return MP_OKAY;
   }
   if (a->used == 1) {
      if (a->dp[0] == 1) {
         if ((e = mp_set_int(c,1)) != MP_OKAY) {
            return e;
         }
      }
      return MP_OKAY;
   }

   /* rounds the edges of the stair a bit */
   /*
   il2 = ilog2((unsigned int)a->used);
   if( a->used > powtwos[il2]  &&  a->used < powtwos[il2]+powtwos[il2-2]){
     return mp_toom_cook_5_sqr(a,c);
   }
   */
   if ((e = mp_dp_to_fft_single(a, &fa, &length)) != MP_OKAY) {
      if (fa != NULL) free(fa);
      return e;
   }
   if ((e = mp_fft_sqr_d(fa, length)) != MP_OKAY) {
      if (fa != NULL) free(fa);
      return e;
   }
   if ((e = mp_fft_to_dp(fa, c, length)) != MP_OKAY) {
      if (fa != NULL) free(fa);
      return e;
   }
   free(fa);
   return MP_OKAY;
}


#else
#error only for 28 bit digits for now
#endif

#endif
