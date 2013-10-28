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

#ifndef  MP_28BIT
  #error only for 28 bit digits for now
#endif

int mp_fft_sqr(mp_int *a,mp_int *c){
  double *fa, *fb;
  fa = NULL;fb = NULL;
  int length, e;
  if(mp_iszero(a) == MP_YES){
    return MP_OKAY; 
  }
  if(a->used == 1 ){
    if(a->dp[0] == 1){
      if( ( e = mp_set_int(c,1) ) != MP_OKAY){
        return e;
      }
    }
    return MP_OKAY; 
  }

  if( ( e = mp_dp_to_fft_single(a, &fa, &length)) != MP_OKAY){
    if(fa != NULL) free(fa);
    return e;
  }
  if( ( e = mp_fft_sqr_d(fa, length)) != MP_OKAY){
    if(fa != NULL) free(fa);
    return e;
  }
  if( ( e = mp_fft_to_dp(fa, c, length)) != MP_OKAY){
    if(fa != NULL) free(fa);
    return e;
  } 
  free(fa);
  return MP_OKAY;
}

#endif
