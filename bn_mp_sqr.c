#include <tommath.h>
#ifdef BN_MP_SQR_C
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

#ifdef BN_MP_FFT_SQR_C


/* Quite rough, assuming a lot */
#if _WIN32 || _WIN64
#   if _WIN64
#      define _ITS_64_BIT_
#   else
#      define _ITS_32_BIT_
#   endif
#elif __GNUC__
#   if __x86_64__ || __ppc64__
#      define _ITS_64_BIT_
#   else
#      define _ITS_32_BIT_
#   endif
#endif

#include <stdint.h>
#if (defined _ITS_64_BIT_) && (!(defined __LP64__))
static const int tab64[64] = {
   63, 0, 58, 1, 59, 47, 53, 2,
   60, 39, 48, 27, 54, 33, 42, 3,
   61, 51, 37, 40, 49, 18, 28, 20,
   55, 30, 34, 11, 43, 14, 22, 4,
   62, 57, 46, 52, 38, 26, 32, 41,
   50, 36, 17, 19, 29, 10, 13, 21,
   56, 45, 25, 31, 35, 16, 9, 12,
   44, 24, 15, 8, 23, 7, 6, 5
};
static int ilog2(uint64_t value)
{
   value |= value >> 1;
   value |= value >> 2;
   value |= value >> 4;
   value |= value >> 8;
   value |= value >> 16;
   value |= value >> 32;
   return tab64[((uint64_t)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >>
                58];
}
#else
static const int tab32[32] = {
   0, 9, 1, 10, 13, 21, 2, 29,
   11, 14, 16, 18, 22, 25, 3, 30,
   8, 12, 20, 28, 15, 17, 24, 7,
   19, 27, 23, 6, 26, 5, 4, 31
};
static int ilog2(uint32_t value)
{
   value |= value >> 1;
   value |= value >> 2;
   value |= value >> 4;
   value |= value >> 8;
   value |= value >> 16;
   return tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
}
#endif

static const int powtwos[] = {
   1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
   8192, 16384, 32768, 65536, 131072, 262144, 524288,
   1048576, 2097152, 4194304, 8388608, 16777216, 33554432,
   67108864, 134217728, 268435456, 536870912, 1073741824
};

static const int powextr[] = {
   1, 2, 4, 8, 16, 32, 64, 128, 256, 768, 1280, 2304,6142,
   8192,16384, 32768 , 65536, 131072, 262144, 524288,
   1048576, 2097152, 4194304, 8388608, 16777216, 33554432,
   67108864, 134217728, 268435456, 536870912, 1073741824
};

#endif

/* computes b = a*a */
int
mp_sqr(mp_int *a, mp_int *b)
{
   int     res;
#ifdef BN_MP_FFT_SQR_C
   int il2,extra;
#endif

#ifdef  MP_28BIT
   /* use FFT? */
#ifdef BN_MP_FFT_SQR_C
   /*
      FFT has an upper limit caused by rounding erors. After one round of
      Toom-Cook it can cut in again.
    */
   if (a->used >= FFT_SQR_CUTOFF &&
       a->used <= FFT_UPPER_LIMIT) {
      /* rounds the edges of the stair a bit */
      il2 = ilog2((unsigned int) a->used);
      extra = 256;//(il2<12)?powtwos[il2-2]:powtwos[8];
      if (a->used >  powtwos[il2]  && a->used  < powextr[il2]) {
         goto town;
      }
      res = mp_fft_sqr(a, b);
   } else
#endif
#endif
town:
#ifdef BN_MP_TOOM_COOK_5_SQR_C
      if (a->used >= TOOM_COOK_5_SQR_CO) {
         res = mp_toom_cook_5_sqr(a, b);
      } else
#endif

#ifdef BN_MP_TOOM_COOK_4_SQR_C
         if (a->used >= TOOM_COOK_4_SQR_CO) {
            res = mp_toom_cook_4_sqr(a, b);
         } else
#endif

#ifdef BN_MP_TOOM_SQR
            /* use Toom-Cook? */
            if (a->used >= TOOM_SQR_CUTOFF) {
               res = mp_toom_sqr(a, b);
               /* Karatsuba? */
            } else
#endif
#ifdef BN_MP_KARATSUBA_SQR_C
               if (a->used >= KARATSUBA_SQR_CUTOFF) {
                  res = mp_karatsuba_sqr(a, b);
               } else
#endif
               {
#ifdef BN_FAST_S_MP_SQR_C
                  /* can we use the fast comba multiplier? */
                  if ((a->used * 2 + 1) < MP_WARRAY &&
                      a->used <
                      (1 << (sizeof(mp_word) * CHAR_BIT - 2*DIGIT_BIT - 1))) {
                     res = fast_s_mp_sqr(a, b);
                  } else
#endif
#ifdef BN_S_MP_SQR_C
                     res = s_mp_sqr(a, b);
#else
                     res = MP_VAL;
#endif
               }
   b->sign = MP_ZPOS;
   return res;
}
#endif

/* $Source$ */
/* $Revision: 0.41 $ */
/* $Date: 2007-04-18 09:58:18 +0000 $ */
