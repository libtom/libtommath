#include "tommath_private.h"
#ifdef BN_S_MP_FFT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* base two integer logarithm */
static int s_highbit(int n)
{
   int r=0;
   int m=n;
   while (m >>= 1) {
      r++;
   }
   return r;
}

/* No libmath allowed, we have to do it manually. */
# define MP_M_PI 3.14159265358979323846

#define MP_DOUBLE_MANTISSA_MAX 0x20000000000000p0
#define MP_ISNAN(x)  ((x) != (x))

static double s_floor(double x);
static double s_ceil(double x);
#if (MP_DIGIT_BIT == 7)
static uint32_t s_l_round(double x);
#else
static mp_word s_round(double x);
#endif
static double s_sin(unsigned long denom, int point_five);

static double s_floor(double x)
{
   double y;
   if (MP_ISNAN(x) || (x >= MP_DOUBLE_MANTISSA_MAX)) {
      return x;
   } else if (x > 0.0 && x < 1.0) {
      return 0.0;
   } else if (x > -1.0 && x < 0.0) {
      return -1.0;
   } else if (x < 0.0) {
      return -s_ceil(-x);
   } else {
      y = x + MP_DOUBLE_MANTISSA_MAX;
      y -= MP_DOUBLE_MANTISSA_MAX;
   }
   return (x < y) ? y - 1.0 : y;
}

static double s_ceil(double x)
{
   double y;
   if (MP_ISNAN(x) || (x >= MP_DOUBLE_MANTISSA_MAX)) {
      return x;
   } else if (x > 0.0 && x < 1.0) {
      return 1.0;
   } else if (x > -1.0 && x < 0.0) {
      return 0.0;
   } else if (x < 0.0) {
      return -s_floor(-x);
   } else {
      y = x + MP_DOUBLE_MANTISSA_MAX;
      y -= MP_DOUBLE_MANTISSA_MAX;
   }
   return (x < y) ? y + 1.0 : y;
}
#if (MP_DIGIT_BIT == 7)
static uint32_t s_l_round(double x)
{
   double d;
   d = s_floor(x);
   if (x - d >= 0.5) {
      d += 1.0;
   }
   return (uint32_t)d;
}
#else
static mp_word s_round(double x)
{
   double d;
   d = s_floor(x);
   if (x - d >= 0.5) {
      d += 1.0;
   }
   return (mp_word)d;
}
#endif
/*
   The sine function.
   We don't need any reduction. With the exception of a handful of values
   all input is in one quadrant and smaller than 1/4 Pi. A small table and
   some terms of the Taylor series will do.
*/
static double s_sin_remez(double input, double d)
{
   double term;
   /* min-max computed with lolremez https://github.com/samhocevar/lolremez */
   term = -8.8116933007339306e-29;
   term = term * d + 6.4411965944908064e-26;
   term = term * d + -3.8681152307258885e-23;
   term = term * d + 1.9572937452212e-20;
   term = term * d + -8.2206352293511003e-18;
   term = term * d + 2.8114572542835332e-15;
   term = term * d + -7.6471637318181213e-13;
   term = term * d + 1.6059043836821579e-10;
   term = term * d + -2.5052108385441718e-8;
   term = term * d + 2.7557319223985891e-6;
   term = term * d + -1.9841269841269841e-4;
   term = term * d + 8.3333333333333333e-3;
   term = term * d + -1.6666666666666667e-1;
   term = term * d + 1.0;

   return input * term;
}
#define MP_SINE_FULL 0
#define MP_SINE_HALF 1
static double s_sin(unsigned long denom, int point_five)
{
   double half = 5.00000000000000000000e-01;
   double input, tmp;
   const double precomp_full[] = {
      -1.0,
      0.0,                                      /* sin(pi/1) */
      1.0,                                      /* sin(pi/2) */
      0.86602540378443864676372317075293618347, /* sin(pi/3) */
      0.70710678118654752440084436210484903928, /* sin(pi/4) */
      0.58778525229247312916870595463907276860, /* sin(pi/5) */
      5.00000000000000000000e-01,               /* sin(pi/6) */
      0.43388373911755812047576833284835875461, /* sin(pi/7) */
      0.38268343236508977172845998403039886676, /* sin(pi/8) */
      0.34202014332566873304409961468225958076, /* sin(pi/9) */
      0.30901699437494742410229341718281905886, /* sin(pi/10) */
   };
   const double precomp_half[] = {
      -1.0,
      1.0,                                      /* sin(0.5 * pi/1) */
      0.70710678118654752440084436210484903928, /* sin(0.5 * pi/2) */
      5.00000000000000000000e-01,               /* sin(0.5 * pi/3) */
      0.38268343236508977172845998403039886676, /* sin(0.5 * pi/4) */
      0.30901699437494742410229341718281905886, /* sin(0.5 * pi/5) */
      0.25881904510252076234889883762404832835, /* sin(0.5 * pi/6) */
      0.22252093395631440428890256449679475947, /* sin(0.5 * pi/7) */
      0.19509032201612826784828486847702224093, /* sin(0.5 * pi/8) */
      0.17364817766693034885171662676931479600, /* sin(0.5 * pi/9) */
      0.15643446504023086901010531946716689231, /* sin(0.5 * pi/10) */
   };
   if (denom <= 10UL) {
      return (point_five == MP_SINE_HALF)?precomp_half[denom]:precomp_full[denom];
   }

   input = (point_five == MP_SINE_HALF)? half * (MP_M_PI / (double)denom): MP_M_PI / (double)denom;
   tmp = input * input;

   return s_sin_remez(input, tmp);
}

/* Convolution - core functions */

#if (MP_DIGIT_BIT == 7) /* MP_8BIT */
/* 7 bit limbs? We can use them all at once, no splitting! */
#define MP_DIGIT_SIZE ((mp_digit)(1) << MP_DIGIT_BIT)
/* Transform multiplicands into floating point numbers */
int s_mp_dp_to_fft(const mp_int *a, double **fa, const mp_int *b, double **fb, int *length)
{
   int length_a, length_b, length_needed, i, hb;
   double *fft_array_a,*fft_array_b;

   /* Check of the multiplicands happens earlier */
   length_a = a->used;
   length_b = b->used;

   length_needed = length_a + length_b;
   /* final length must be a power of two to keep the FFTs simple */
   hb = s_highbit(length_needed);
   /* check for the rare case that it is already a power of 2 */
   if (length_needed != (1 << hb)) {
      length_needed = 1 << (hb+1);
   }
   /* Send computed length back to caller */
   *length = length_needed;

   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }
   fft_array_b = MP_MALLOC(sizeof(double) * (size_t)length_needed);
   if (fft_array_b == NULL) {
      return MP_MEM;
   }
   /* Put digits in double-array, in the same order as in mp_int */
   for (i = 0; i < length_needed; i++) {
      if (i < length_a) {
         fft_array_a[i]   = (double)(a->dp[i]);
      }
      /* padding a */
      if (i >= length_a) {
         fft_array_a[i]   = 0.0;
      }
      if (i < length_b) {
         fft_array_b[i]   = (double)(b->dp[i]);
      }
      /* padding b */
      if (i >= length_b) {
         fft_array_b[i]   = 0.0;
      }
   }

   /* Send the route to memory back to caller */
   *fa = fft_array_a;
   *fb = fft_array_b;
   return MP_OKAY;
}

/* same as dp_to_fft() for a single multiplicand for squaring */
int s_mp_dp_to_fft_single(const mp_int *a, double **fa, int *length)
{
   int length_a,  length_needed, i, hb;
   double *fft_array_a;
   length_a = a->used;
   length_needed = length_a * 2;
   hb = s_highbit(length_needed);
   if (length_needed != (1 << hb)) {
      length_needed = 1 << (hb+1);
   }
   *length = length_needed;
   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }
   for (i = 0; i < length_needed; i++) {
      if (i < length_a) {
         fft_array_a[i] = (double)(a->dp[i]);
      }
      if (i >= length_a) {
         fft_array_a[i] = 0.0;
      }
   }
   *fa = fft_array_a;
   return MP_OKAY;
}

/* Because we use the full limb, mp_word is not large enough anymore */
int s_mp_fft_to_dp(double *fft_array, mp_int *a, int length, mp_word *cy)
{
   int i, e;
   uint32_t carry, temp;

   /* Preallocate some memory for the result. */
   if (a->alloc < length) {
      if ((e = mp_grow(a, length + 1)) != MP_OKAY) {
         return e;
      }
   }
   /* The FFT multiplication does no carry */
   carry = 0uL;
   for (i = 0; i < length; i++) {
      temp = carry;
      carry = 0uL;
      temp  += (uint32_t)(s_l_round(fft_array[i]));
      if (temp >= (uint32_t)MP_DIGIT_SIZE) {
         carry = temp / (uint32_t)MP_DIGIT_SIZE;
         temp  = temp & (uint32_t)MP_MASK;
      }
      /* memory is still expensive, not a thing to waste easily */
      fft_array[i] = (double)temp;
   }
   /* re-marry the digits */
   for (i = 0; i < length; i++) {
      a->dp[i]  = (mp_digit)(s_l_round(fft_array[i])) & MP_MASK;
      /* and count them all */
      a->used++;
   }
   if (carry) {
      a->dp[i] = (mp_digit)carry;
      a->used++;
      *cy = carry;
   }
   mp_clamp(a);
   return MP_OKAY;
}

#elif (MP_DIGIT_BIT == 15) /* MP_16BIT */
#define MP_DIGIT_SIZE ((mp_digit)(1) << MP_DIGIT_BIT)
#define MP_DIGIT_BIT_THIRD (MP_DIGIT_BIT / 3)
#define MP_DIGIT_THIRD ((mp_digit)(1) << MP_DIGIT_BIT_THIRD )
#define MP_DIGIT_MASK (MP_DIGIT_THIRD - 1)

/* Transform multiplicands into floating point numbers with third sized digits*/
int s_mp_dp_to_fft(const mp_int *a, double **fa, const mp_int *b, double **fb, int *length)
{
   int length_a, length_b, length_needed, i, hb, rest;
   double *fft_array_a,*fft_array_b;

   length_a = a->used;
   length_b = b->used;

   /* Digits get split in three, so three times the length is needed*/
   length_needed = (length_a + length_b) * 3;
   hb = s_highbit(length_needed);
   if (length_needed != (1 << hb)) {
      length_needed = 1 << (hb+1);
   }

   *length = length_needed;

   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)(length_needed + 3));
   if (fft_array_a == NULL) {
      return MP_MEM;
   }
   fft_array_b = MP_MALLOC(sizeof(double) * (size_t)(length_needed + 3));
   if (fft_array_b == NULL) {
      return MP_MEM;
   }

   for (i = 0; i < (length_needed / 3); i++) {
      if (i < length_a) {
         fft_array_a[(3*i)]   = (double)(a->dp[i]                             & MP_DIGIT_MASK);
         fft_array_a[(3*i)+1] = (double)((a->dp[i] >>    MP_DIGIT_BIT_THIRD)  & MP_DIGIT_MASK);
         fft_array_a[(3*i)+2] = (double)((a->dp[i] >> (2*MP_DIGIT_BIT_THIRD)) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[(3*i)]   = 0.0;
         fft_array_a[(3*i)+1] = 0.0;
         fft_array_a[(3*i)+2] = 0.0;
      }
      if (i < length_b) {
         fft_array_b[(3*i)]   = (double)(b->dp[i]                            & MP_DIGIT_MASK);
         fft_array_b[(3*i)+1] = (double)((b->dp[i] >>    MP_DIGIT_BIT_THIRD)  & MP_DIGIT_MASK);
         fft_array_b[(3*i)+2] = (double)((b->dp[i] >> (2*MP_DIGIT_BIT_THIRD)) & MP_DIGIT_MASK);
      }
      if (i >= length_b) {
         fft_array_b[(3*i)]   = 0.0;
         fft_array_b[(3*i)+1] = 0.0;
         fft_array_b[(3*i)+2] = 0.0;
      }
   }

   rest = (length_needed / 3) * 3;
   for (i=rest; i<length_needed + 3; i++) {
      fft_array_a[i] = 0.0;
      fft_array_b[i] = 0.0;
   }

   *fa = fft_array_a;
   *fb = fft_array_b;
   return MP_OKAY;
}

int s_mp_dp_to_fft_single(const mp_int *a, double **fa, int *length)
{
   int length_a,  length_needed, i, hb, rest;
   double *fft_array_a;
   length_a = a->used;
   length_needed = (length_a * 2) * 3 ;
   hb = s_highbit(length_needed);
   if (length_needed != (1 << hb)) {
      length_needed = 1 << (hb+1);
   }
   *length = length_needed;
   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)(length_needed + 3));
   if (fft_array_a == NULL) {
      return MP_MEM;
   }

   for (i = 0; i < (length_needed / 3); i++) {
      if (i < length_a) {
         fft_array_a[(3*i)]   = (double)(a->dp[i]                            & MP_DIGIT_MASK);
         fft_array_a[(3*i)+1] = (double)((a->dp[i] >>    MP_DIGIT_BIT_THIRD)  & MP_DIGIT_MASK);
         fft_array_a[(3*i)+2] = (double)((a->dp[i] >> (2*MP_DIGIT_BIT_THIRD)) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[(3*i)]   = 0.0;
         fft_array_a[(3*i)+1] = 0.0;
         fft_array_a[(3*i)+2] = 0.0;
      }
   }

   rest = (length_needed / 3) * 3;
   for (i=rest; i < (length_needed + 3); i++) {
      fft_array_a[i] = 0.0;
   }
   *fa = fft_array_a;
   return MP_OKAY;
}

int s_mp_fft_to_dp(double *fft_array, mp_int *a,int length, mp_word *cy)
{
   int i, j, e;
   mp_word carry,temp;

   if (a->alloc < length) {
      if ((e = mp_grow(a, length + 3)) != MP_OKAY) {
         return e;
      }
   }
   carry = (mp_word)(0);
   for (i = 0; i < length; i++) {
      temp = carry;
      carry = (mp_word)(0);
      temp  += (mp_word)(s_round(fft_array[i]));
      if (temp >= MP_DIGIT_THIRD) {
         carry = temp / (mp_word)MP_DIGIT_THIRD;
         temp  = temp & (mp_word)MP_DIGIT_MASK;
      }
      fft_array[i] = (double)temp;
   }

   for (i = 0, j = 0; j < length; i++, j += 3) {
      a->dp[i] |= (mp_digit)(s_round(fft_array[j+2])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_THIRD;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j+1])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_THIRD;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j]))   & MP_DIGIT_MASK;
      a->used++;
   }

   if (carry) {
      a->dp[i] = (mp_digit)carry;
      a->used++;
      *cy = carry;
   }
   mp_clamp(a);
   return MP_OKAY;
}

#elif (MP_DIGIT_BIT == 28) /* MP_28BIT */
/*
   Quartering the 28 bit limbs needs twice as much memory and is a little bit slower
   but no rounding errors were found below 10 million limbs.
 */
#ifdef MP_FFT_QUARTER_28
#define MP_DIGIT_SIZE ((mp_digit)(1) << MP_DIGIT_BIT)
#define MP_DIGIT_BIT_QUARTER (MP_DIGIT_BIT >> 2)
#define MP_DIGIT_QUARTER (1L << MP_DIGIT_BIT_QUARTER )
#define MP_DIGIT_MASK (MP_DIGIT_QUARTER - 1)

/* Transform multiplicands into floating point numbers with quarter sized digits*/
int s_mp_dp_to_fft(const mp_int *a, double **fa, const mp_int *b, double **fb, int *length)
{
   int length_a, length_b, length_needed, i, hb;
   double *fft_array_a,*fft_array_b;

   length_a = a->used;
   length_b = b->used;

   /* Digits get split in four, so four times the length is needed*/
   length_needed = ((length_a + length_b)) * 4;
   hb = s_highbit(length_needed);
   if (length_needed != (1 << hb)) {
      length_needed = 1 << (hb+1);
   }

   *length = length_needed;

   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }
   fft_array_b = MP_MALLOC(sizeof(double) * (size_t)length_needed);
   if (fft_array_b == NULL) {
      return MP_MEM;
   }

   for (i = 0; i < (length_needed / 4); i++) {
      if (i < length_a) {
         fft_array_a[(4*i)]   = (double)(a->dp[i]                               & MP_DIGIT_MASK);
         fft_array_a[(4*i)+1] = (double)((a->dp[i] >>    MP_DIGIT_BIT_QUARTER)  & MP_DIGIT_MASK);
         fft_array_a[(4*i)+2] = (double)((a->dp[i] >> (2*MP_DIGIT_BIT_QUARTER)) & MP_DIGIT_MASK);
         fft_array_a[(4*i)+3] = (double)((a->dp[i] >> (3*MP_DIGIT_BIT_QUARTER)) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[(4*i)]   = 0.0;
         fft_array_a[(4*i)+1] = 0.0;
         fft_array_a[(4*i)+2] = 0.0;
         fft_array_a[(4*i)+3] = 0.0;
      }
      if (i < length_b) {
         fft_array_b[(4*i)]   = (double)(b->dp[i]                               & MP_DIGIT_MASK);
         fft_array_b[(4*i)+1] = (double)((b->dp[i] >>    MP_DIGIT_BIT_QUARTER)  & MP_DIGIT_MASK);
         fft_array_b[(4*i)+2] = (double)((b->dp[i] >> (2*MP_DIGIT_BIT_QUARTER)) & MP_DIGIT_MASK);
         fft_array_b[(4*i)+3] = (double)((b->dp[i] >> (3*MP_DIGIT_BIT_QUARTER)) & MP_DIGIT_MASK);
      }
      if (i >= length_b) {
         fft_array_b[(4*i)]   = 0.0;
         fft_array_b[(4*i)+1] = 0.0;
         fft_array_b[(4*i)+2] = 0.0;
         fft_array_b[(4*i)+3] = 0.0;
      }
   }

   *fa = fft_array_a;
   *fb = fft_array_b;
   return MP_OKAY;
}

int s_mp_dp_to_fft_single(const mp_int *a, double **fa, int *length)
{
   int length_a,  length_needed, i, hb;
   double *fft_array_a;
   length_a = a->used;
   length_needed = (length_a * 2) * 4 ;
   hb = s_highbit(length_needed);
   if (length_needed != (1 << hb)) {
      length_needed = 1 << (hb+1);
   }
   *length = length_needed;
   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }

   for (i = 0; i < (length_needed / 4); i++) {
      if (i < length_a) {
         fft_array_a[(4*i)]   = (double)(a->dp[i]                               & MP_DIGIT_MASK);
         fft_array_a[(4*i)+1] = (double)((a->dp[i] >>    MP_DIGIT_BIT_QUARTER)  & MP_DIGIT_MASK);
         fft_array_a[(4*i)+2] = (double)((a->dp[i] >> (2*MP_DIGIT_BIT_QUARTER)) & MP_DIGIT_MASK);
         fft_array_a[(4*i)+3] = (double)((a->dp[i] >> (3*MP_DIGIT_BIT_QUARTER)) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[(4*i)]   = 0.0;
         fft_array_a[(4*i)+1] = 0.0;
         fft_array_a[(4*i)+2] = 0.0;
         fft_array_a[(4*i)+3] = 0.0;
      }
   }

   *fa = fft_array_a;
   return MP_OKAY;
}

int s_mp_fft_to_dp(double *fft_array, mp_int *a, int length, mp_word *cy)
{
   int i,j,e;
   mp_word carry,temp;

   if (a->alloc < length) {
      if ((e = mp_grow(a, length + 1)) != MP_OKAY) {
         return e;
      }
   }

   carry = (mp_word)(0);
   for (i = 0; i<length; i++) {
      temp = carry;
      carry = (mp_word)(0);
      temp  += (mp_word)(s_round(fft_array[i]));
      if (temp >= MP_DIGIT_QUARTER) {
         carry = temp / (mp_word)MP_DIGIT_QUARTER;
         temp  = temp & (mp_word)MP_DIGIT_MASK;
      }
      fft_array[i] = (double)temp;
   }
   for (i = 0, j = 0; j < length; i++, j += 4) {
      a->dp[i] |= (mp_digit)(s_round(fft_array[j+3])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_QUARTER;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j+2])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_QUARTER;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j+1])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_QUARTER;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j]))   & MP_DIGIT_MASK;
      a->used++;
   }

   if (carry) {
      a->dp[i] = (mp_digit)carry;
      a->used++;
      *cy = carry;
   }
   mp_clamp(a);
   return MP_OKAY;
}


#else /* MP_FFT_QUARTER_28 */
#define MP_DIGIT_SIZE ((mp_digit)(1) << MP_DIGIT_BIT)
#define MP_DIGIT_BIT_HALF (MP_DIGIT_BIT >> 1)
#define MP_DIGIT_HALF ((mp_digit)(1) << MP_DIGIT_BIT_HALF )
#define MP_DIGIT_MASK (MP_DIGIT_HALF - 1)

/* Transform multiplicands into floating point numbers with half sized digits*/
int s_mp_dp_to_fft(const mp_int *a, double **fa, const mp_int *b, double **fb, int *length)
{
   int length_a, length_b, length_needed, i,j, hb;
   double *fft_array_a,*fft_array_b;

   length_a = a->used;
   length_b = b->used;

   /* Digits get split in half, so twice the length is needed*/
   length_needed = ((length_a + length_b))*2 ;
   hb = s_highbit(length_needed);
   if (length_needed != (1 << hb)) {
      length_needed = 1 << (hb+1);
   }

   *length = length_needed;

   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }
   fft_array_b = MP_MALLOC(sizeof(double) * (size_t)length_needed);
   if (fft_array_b == NULL) {
      return MP_MEM;
   }

   for (i = 0, j = 0; i<length_needed/2; i++, j += 2) {
      if (i < length_a) {
         fft_array_a[j]   = (double)(a->dp[i]                        & MP_DIGIT_MASK);
         fft_array_a[j+1] = (double)((a->dp[i] >> MP_DIGIT_BIT_HALF) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[j]   = 0.0;
         fft_array_a[j+1] = 0.0;
      }
      if (i < length_b) {
         fft_array_b[j]   = (double)(b->dp[i]                        & MP_DIGIT_MASK);
         fft_array_b[j+1] = (double)((b->dp[i] >> MP_DIGIT_BIT_HALF) & MP_DIGIT_MASK);
      }
      if (i >= length_b) {
         fft_array_b[j]   = 0.0;
         fft_array_b[j+1] = 0.0;
      }
   }

   *fa = fft_array_a;
   *fb = fft_array_b;
   return MP_OKAY;
}

int s_mp_dp_to_fft_single(const mp_int *a, double **fa, int *length)
{
   int length_a,  length_needed, i,j, hb;
   double *fft_array_a;
   length_a = a->used;
   length_needed = (length_a * 2) * 2 ;
   hb = s_highbit(length_needed);
   if (length_needed != (1 << hb)) {
      length_needed = 1 << (hb+1);
   }
   *length = length_needed;
   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }

   for (i = 0, j = 0; i < (length_needed / 2); i++, j += 2) {
      if (i < length_a) {
         fft_array_a[j]   = (double)(a->dp[i]                        & MP_DIGIT_MASK);
         fft_array_a[j+1] = (double)((a->dp[i] >> MP_DIGIT_BIT_HALF) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[j]   = 0.0;
         fft_array_a[j+1] = 0.0;
      }
   }

   *fa = fft_array_a;
   return MP_OKAY;
}

int s_mp_fft_to_dp(double *fft_array, mp_int *a, int length, mp_word *cy)
{
   int i,j,e;
   mp_word carry,temp;

   if (a->alloc < length) {
      if ((e = mp_grow(a, length + 1)) != MP_OKAY) {
         return e;
      }
   }

   carry = 0;
   for (i=0; i<length; i++) {
      temp = carry;
      carry = 0;
      temp  += (mp_word)(s_round(fft_array[i]));
      if (temp >= MP_DIGIT_HALF) {
         carry = temp / (mp_word)MP_DIGIT_HALF;
         temp  = temp & (mp_word)MP_DIGIT_MASK;
      }
      fft_array[i] = (double)temp;
   }

   for (i = 0, j = 0; j < length; i++, j += 2) {
      a->dp[i]  = (mp_digit)(s_round(fft_array[j+1])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_HALF;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j]))   & MP_DIGIT_MASK;
      a->used++;
   }

   if (carry) {
      a->dp[i] = (mp_digit)carry;
      a->used++;
      *cy = carry;
   }
   mp_clamp(a);
   return MP_OKAY;
}

#endif /* MP_FFT_QUARTER_28 */

#elif (MP_DIGIT_BIT == 31) /* MP_31BIT */
#  ifdef _MSC_VER
#    pragma message("7, 15, 28 and 60 bit limbs only (no 31 bit, sorry).")
#  else
#    warning "7, 15, 28 and 60 bit limbs only (no 31 bit, sorry)."
#  endif

#elif (MP_DIGIT_BIT == 60) /* MP_64BIT */
#define MP_DIGIT_SIZE ((mp_digit)(1) << MP_DIGIT_BIT)
#define MP_DIGIT_BIT_FIFTH (MP_DIGIT_BIT / 5)
#define MP_DIGIT_FIFTH ((mp_digit)(1) << MP_DIGIT_BIT_FIFTH )
#define MP_DIGIT_MASK (MP_DIGIT_FIFTH-1)

/* Transform multiplicands into floating point numbers with fifth sized digits*/
int s_mp_dp_to_fft(const mp_int *a, double **fa, const mp_int *b, double **fb, int *length)
{
   int length_a, length_b, length_needed, i, hb, rest;
   double *fft_array_a,*fft_array_b;

   length_a = a->used;
   length_b = b->used;

   /* Digits get split in fifths, so five times the length is needed*/
   length_needed = (length_a + length_b) * 5;
   hb = s_highbit(length_needed);
   if (length_needed != (1 << hb)) {
      length_needed = 1 << (hb+1);
   }

   *length = length_needed;

   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)(length_needed + 5));
   if (fft_array_a == NULL) {
      return MP_MEM;
   }

   fft_array_b = MP_MALLOC(sizeof(double) * (size_t)(length_needed + 5));
   if (fft_array_b == NULL) {
      return MP_MEM;
   }

   for (i = 0; i < (length_needed / 5); i++) {
      if (i < length_a) {
         fft_array_a[(5*i)]   = (double)(a->dp[i]                             & MP_DIGIT_MASK);
         fft_array_a[(5*i)+1] = (double)((a->dp[i] >>    MP_DIGIT_BIT_FIFTH)  & MP_DIGIT_MASK);
         fft_array_a[(5*i)+2] = (double)((a->dp[i] >> (2*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_a[(5*i)+3] = (double)((a->dp[i] >> (3*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_a[(5*i)+4] = (double)((a->dp[i] >> (4*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[(5*i)]   = 0.0;
         fft_array_a[(5*i)+1] = 0.0;
         fft_array_a[(5*i)+2] = 0.0;
         fft_array_a[(5*i)+3] = 0.0;
         fft_array_a[(5*i)+4] = 0.0;
      }
      if (i < length_b) {
         fft_array_b[(5*i)]   = (double)(b->dp[i]                             & MP_DIGIT_MASK);
         fft_array_b[(5*i)+1] = (double)((b->dp[i] >>    MP_DIGIT_BIT_FIFTH)  & MP_DIGIT_MASK);
         fft_array_b[(5*i)+2] = (double)((b->dp[i] >> (2*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_b[(5*i)+3] = (double)((b->dp[i] >> (3*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_b[(5*i)+4] = (double)((b->dp[i] >> (4*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
      }
      if (i >= length_b) {
         fft_array_b[(5*i)]   = 0.0;
         fft_array_b[(5*i)+1] = 0.0;
         fft_array_b[(5*i)+2] = 0.0;
         fft_array_b[(5*i)+3] = 0.0;
         fft_array_b[(5*i)+4] = 0.0;
      }
   }

   /* there is a small problem with divisibility of 2^n and 5, so ... */
   rest = (length_needed/5)*5;
   for (i=rest; i<length_needed + 5; i++) {
      fft_array_a[i] = 0.0;
      fft_array_b[i] = 0.0;
   }

   *fa = fft_array_a;
   *fb = fft_array_b;
   return MP_OKAY;
}


int s_mp_dp_to_fft_single(const mp_int *a, double **fa, int *length)
{
   int length_a,  length_needed, i, hb, rest;
   double *fft_array_a;
   length_a = a->used;
   length_needed = (length_a * 2) * 5;
   hb = s_highbit(length_needed);
   if (length_needed != 1<<hb) {
      length_needed = 1<<(hb+1);
   }
   *length = length_needed;
   fft_array_a = MP_MALLOC(sizeof(double) * (size_t)(length_needed + 5));
   if (fft_array_a == NULL) {
      return MP_MEM;
   }

   for (i = 0; i < (length_needed / 5); i++) {
      if (i < length_a) {
         fft_array_a[(5*i)]   = (double)(a->dp[i]                             & MP_DIGIT_MASK);
         fft_array_a[(5*i)+1] = (double)((a->dp[i] >>    MP_DIGIT_BIT_FIFTH)  & MP_DIGIT_MASK);
         fft_array_a[(5*i)+2] = (double)((a->dp[i] >> (2*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_a[(5*i)+3] = (double)((a->dp[i] >> (3*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_a[(5*i)+4] = (double)((a->dp[i] >> (4*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[(5*i)]   = 0.0;
         fft_array_a[(5*i)+1] = 0.0;
         fft_array_a[(5*i)+2] = 0.0;
         fft_array_a[(5*i)+3] = 0.0;
         fft_array_a[(5*i)+4] = 0.0;
      }
   }

   rest = (length_needed / 5) * 5;
   for (i = rest; i < length_needed + 5; i++) {
      fft_array_a[i] = 0.0;
   }
   *fa = fft_array_a;
   return MP_OKAY;
}

int s_mp_fft_to_dp(double *fft_array, mp_int *a,int length, mp_word *cy)
{
   int i,j,e;
   mp_word carry = 0,temp;

   if (a->alloc < length) {
      if ((e = mp_grow(a, length + 1)) != MP_OKAY) {
         return e;
      }
   }

   for (i=0; i<length; i++) {
      temp = carry;
      carry = 0;
      temp  += (mp_word)(s_round(fft_array[i]));
      if (temp >= MP_DIGIT_FIFTH) {
         carry = temp / (mp_word)MP_DIGIT_FIFTH;
         temp  = temp & (mp_word)MP_DIGIT_MASK;
      }
      fft_array[i] = (double)temp;
   }

   for (i = 0, j = 0; j < length; i++, j += 5) {
      a->dp[i]  = (mp_digit)(s_round(fft_array[j+4])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_FIFTH;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j+3])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_FIFTH;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j+2])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_FIFTH;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j+1])) & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_FIFTH;
      a->dp[i] |= (mp_digit)(s_round(fft_array[j]))   & MP_DIGIT_MASK;
      a->used++;
   }

   if (carry) {
      a->dp[i] = (mp_digit)carry;
      a->used++;
      *cy = carry;
   }
   mp_clamp(a);
   return MP_OKAY;
}
#else
#  ifdef _MSC_VER
#    pragma message("7, 15, 28 and 60 bit limbs only (no 31 bit, sorry).")
#  else
#    warning "7, 15, 28 and 60 bit limbs only (no 31 bit, sorry)."
#  endif
#endif

/*
  The size of the L1-cache in bytes. The number here is that of the data cache
  part of an AMD A8-6600K.
  The Linux kernel gives a lot of information e.g.:
    grep . /sys/devices/system/cpu/cpu0/cache/index*//*
  There is also lscpu(1) which is easier to interpret.
  On Windows:
    http://msdn.microsoft.com/en-us/library/ms683194.aspx
    http://www.cpuid.com/softwares/cpu-z.htm
  Lack of access to a Mac leaves that part blank. The new MacOS is based on BSD,
  so 'dmesg' might work or
    cat /var/run/dmesg.boot | grep CPU
  A short search at Google found
    sysctl hw.l1dcachesize
  to get the data L1 cache of a modern Mac (Intel. Haswell, I think)
  Very old Macs don't have much, see e.g.:
    https://en.wikipedia.org/wiki/List_of_Macintosh_models_grouped_by_CPU_type
  for an extensive list.

 */
#ifndef L1_SIZE
#define L1_SIZE 16384
#endif

/*
  The iterative version of a Hartley transform, decimation in frequency.
  For the technical details see the excellent description in J. Arndt's book
  "Matters Computational" available for no cost at
     http://www.jjj.de/fxt/fxtbook.pdf
  (But it is also available at e.g.: Amazon if you want to support a starving artist)

  See pages 515 ff.

  Optimizations used here:
    - hartley_shift (p. 516 f.) which avoids a large number of trig. comput.

   Also: the trig. values can be pre-computed but that is not done here.
 */
static void s_fht_dif_iterative(double *x, unsigned long n, int do_loop)
{
   unsigned long m, mh, mq;
   unsigned long i, j, k;
   double a, b, c, s, u, v, tmp;
   double *dp;
   for (m = n; m > 1; m >>= 1) {
      mh = m >> 1;
      mq = mh >> 1;
      a = s_sin(mh, MP_SINE_HALF); /* cosine */
      a *= 2.0 * a;
      b = s_sin(mh, MP_SINE_FULL);
      for (i = 0; i < n; i += m) {
         dp = x + i;
         for (j = 0, k = mh; j < mh; ++j, ++k) {
            u = dp[j];
            v = dp[k];
            dp[j] = u + v;
            dp[k] = u - v;
         }
         dp += mh;
         c = 1.0;
         s = 0.0;
         for (j = 1, k = mh - 1; j < mq; ++j, --k) {
            tmp = c;
            c -= a * c + b * s;
            s -= a * s - b * tmp;
            u = dp[j];
            v = dp[k];
            dp[j] = u * c + v * s;
            dp[k] = u * s - v * c;
         }
      }
      if (!do_loop) {
         break;
      }
   }
   return;
}

/*
 * See e.g.:
 * Singleton, R. C., "On Computing the Fast Fourier Transform,"
 * Communications of the ACM 10:647, 1967
 *
 * Seeking for this citation online found the software "am" at
 * https://www.cfa.harvard.edu/~spaine/am/ where the same method (also
 * based on J. Arndt's book) can be found in
 * https://www.cfa.harvard.edu/~spaine/am/download/src/transform.c
 *
 * They implemented it slightly different which might be a better fit
 * for your needs. They also have versions with OMP hooks if you need some
 * parallel support.
 * But, please be aware that the license of "am" is not fully Public Domain
 * in the legal sense, you need to add a line to indicate where you've
 * stolen it.
 * https://www.cfa.harvard.edu/~spaine/am/download/src/LICENSE
 * So if you want it PD you have to take this one here, sorry.
 */

static void s_fht_dif_rec(double *x, unsigned long n)
{
   unsigned long nh;
   if (n == 1) {
      return;
   }
   if (n < (unsigned long)(L1_SIZE / (2 * sizeof(double)))) {
      s_fht_dif_iterative(x, n, 1);
      return;
   }
   s_fht_dif_iterative(x, n, 0);
   nh = n >> 1;
   s_fht_dif_rec(x, nh);
   s_fht_dif_rec(x + nh, nh);
   return;
}


/* The iterative Hartley transform, decimation in time. Description above */
static void s_fht_dit_iterative(double *x, unsigned long n, int do_loop)
{
   unsigned long m, mh, mq;
   unsigned long i, j, k;
   double a, b, u, v, c, s, tmp;
   double *dp;

   m = (do_loop) ? 2 : n;
   for (; m <= n; m <<= 1) {
      mh = m >> 1;
      mq = mh >> 1;
      a = s_sin(mh, MP_SINE_HALF); /* cosine */
      a *= 2.0 * a;
      b = s_sin(mh, MP_SINE_FULL);
      for (i = 0; i < n; i += m) {
         dp = x + i + mh;
         c = 1.0;
         s = 0.0;
         for (j = 1, k = mh - 1; j < mq; j++, k--) {
            tmp = c;
            c -= a * c + b * s;
            s -= a * s - b * tmp;
            u = dp[j];
            v = dp[k];
            dp[j] = u * c + v * s;
            dp[k] = u * s - v * c;
         }
         dp -= mh;
         for (j = 0, k = mh; j < mh; j++, k++) {
            u = dp[j];
            v = dp[k];
            dp[j] = u + v;
            dp[k] = u - v;
         }
      }
   }
   return;
}

/* The actual butterfly. Description above */
static void s_fht_dit_rec(double *x, unsigned long n)
{
   unsigned long nh;

   if (n == 1) {
      return;
   }
   if (n < (unsigned long)(L1_SIZE / (2 * sizeof(double)))) {
      s_fht_dit_iterative(x, n, 1);
      return;
   }
   nh = n >> 1;
   s_fht_dit_rec(x, nh);
   s_fht_dit_rec(x + nh, nh);
   s_fht_dit_iterative(x, n, 0);
   return;
}


/*
 * The FHT convolution from J. Arndt's book.
 * The code looks a bit messy but only on the face of it. This method avoids
 * the otherwise costly bit reversing (which is called "revbin" in J. Arndt's
 * book).
 */
static void s_fht_conv_core(double *f, double *g, unsigned long n, double v /*=0.0*/)
{
   unsigned long nh, r, rm, k, km, tr, m;
   double xi, xj, yi, yj;
   if (v == 0.0) {
      v = 1.0 / n;
   }
   g[0] *= (v * f[0]);
   if (n >= 2) {
      g[1] *= (v * f[1]);
   }
   if (n < 4) {
      return;
   }
   v *= 0.5;
   nh = (n >> 1);
   r = nh;
   rm = n - 1;
   xi = f[r];
   xj = f[rm];
   yi = g[r];
   yj = g[rm];
   g[r] = v * ((xi + xj) * yi + (xi - xj) * yj);
   g[rm] = v * ((-xi + xj) * yi + (xi + xj) * yj);

   k = 2;
   km = n - 2;
   while (k < nh) {
      rm -= nh;
      tr = r;
      r ^= nh;
      m = (nh >> 1);
      while (!((r ^= m) & m)) {
         m >>= 1;
      }
      xi = f[r];
      xj = f[rm];
      yi = g[r];
      yj = g[rm];
      g[r] = v * ((xi + xj) * yi + (xi - xj) * yj);
      g[rm] = v * ((-xi + xj) * yi + (xi + xj) * yj);
      km--;
      k++;
      rm += (tr - r);
      r += nh;
      xi = f[r];
      xj = f[rm];
      yi = g[r];
      yj = g[rm];
      g[r] = v * ((xi + xj) * yi + (xi - xj) * yj);
      g[rm] = v * ((-xi + xj) * yi + (xi + xj) * yj);
      km--;
      k++;
   }
   return;

}

/* FHT auto-convolution. Description above */
static void s_fht_autoconv_core(double *f, unsigned long n, double v /*=0.0*/)
{
   unsigned long nh, r, rm, k, km, tr, m;
   double xi, xj, xi2, xj2, xij;
   if (v == 0.0) {
      v = 1.0 / n;
   }
   f[0] *= (v * f[0]);
   if (n >= 2) {
      f[1] *= (v * f[1]);
   }
   if (n < 4) {
      return;
   }
   v *= 0.5;
   nh = (n >> 1);
   r = nh;
   rm = n - 1;
   xi = f[r];
   xj = f[rm];
   xi2 = xi * xi;
   xj2 = xj * xj;
   xij = (2 * xi * xj);
   f[r] = v * (xi2 + xij - xj2);
   f[rm] = v * (-xi2 + xij + xj2);

   k = 2;
   km = n - 2;
   while (k < nh) {
      rm -= nh;
      tr = r;
      r ^= nh;
      m = (nh >> 1);
      while (!((r ^= m) & m)) {
         m >>= 1;
      }
      xi = f[r];
      xj = f[rm];
      xi2 = xi * xi;
      xj2 = xj * xj;
      xij = (2 * xi * xj);
      f[r] = v * (xi2 + xij - xj2);
      f[rm] = v * (-xi2 + xij + xj2);
      km--;
      k++;
      rm += (tr - r);
      r += nh;
      xi = f[r];
      xj = f[rm];
      xi2 = xi * xi;
      xj2 = xj * xj;
      xij = (2 * xi * xj);
      f[r] = v * (xi2 + xij - xj2);
      f[rm] = v * (-xi2 + xij + xj2);
      km--;
      k++;
   }
   return;
}

/* convolution (multiplication) */
int s_mp_fft(double *x, double *y, unsigned long length)
{
   unsigned long n;
   n = (length);
   if (n < 2) {
      return MP_VAL;
   }
   s_fht_dif_rec(x, (n));
   s_fht_dif_rec(y, (n));
   s_fht_conv_core(x, y, (n), 0.0);
   s_fht_dit_rec(y, (n));
   return MP_OKAY;
}

/* auto-convolution (squaring) */
int s_mp_fft_sqr_ex(double *x, unsigned long length)
{
   unsigned long n;
   n = (length);
   if (n < 2) {
      return MP_VAL;
   }
   s_fht_dif_rec(x, (n));
   s_fht_autoconv_core(x, (n), 0.0);
   s_fht_dit_rec(x, (n));
   return MP_OKAY;
}


#endif
