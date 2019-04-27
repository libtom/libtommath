#include "tommath_private.h"
#ifdef BNCORE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Known optimal configurations

 CPU                    /Compiler     /MUL CUTOFF/SQR CUTOFF
-------------------------------------------------------------
 Intel P4 Northwood     /GCC v3.4.1   /        88/       128/LTM 0.32 ;-)
 AMD Athlon64           /GCC v3.4.4   /        80/       120/LTM 0.35

*/

/*
   Values evaluated on an AMD A8-6600K (64-bit).
   Please see etc/tune.c for ways to optimize them for your machine but
   be aware that it may take a long time.
 */
int     KARATSUBA_MUL_CUTOFF = 125,     /* Min. number of digits before Karatsuba multiplication is used. */
        KARATSUBA_SQR_CUTOFF = 150,     /* Min. number of digits before Karatsuba squaring is used. */

        TOOM_MUL_CUTOFF      = 260,
        TOOM_SQR_CUTOFF      = 260;

int TOOM_COOK_4_MUL_CO = 300;
int TOOM_COOK_4_SQR_CO = 300;
int TOOM_COOK_5_MUL_CO = 400;
int TOOM_COOK_5_SQR_CO = 400;

/* These are just some safe values, please read the documentation for how to get your best fit */
#if (MP_DIGIT_BIT == 60)
int FFT_MUL_LOWER_CO = 75000;
int FFT_MUL_UPPER_CO = 400000;

int FFT_SQR_LOWER_CO = 35000;
int FFT_SQR_UPPER_CO = 400000;
#elif (MP_DIGIT_BIT == 28)
#ifdef MP_FFT_QUARTER_28
int FFT_MUL_LOWER_CO = 85000;
int FFT_MUL_UPPER_CO = 10000000; /* actual upper limit is larger than that */

int FFT_SQR_LOWER_CO = 21000;
int FFT_SQR_UPPER_CO = 10000000; /* actual upper limit is larger than that */
#else
int FFT_MUL_LOWER_CO = 2800;
int FFT_MUL_UPPER_CO = 170000;

int FFT_SQR_LOWER_CO = 4500;
int FFT_SQR_UPPER_CO = 170000;
#endif /* MP_FFT_QUARTER_28 */
#elif (MP_DIGIT_BIT == 15)
int FFT_MUL_LOWER_CO = 27000;
int FFT_MUL_UPPER_CO = 10000000; /* actual upper limit is larger than that */

int FFT_SQR_LOWER_CO = 27000;
int FFT_SQR_UPPER_CO = 10000000; /* actual upper limit is larger than that */
#elif (MP_DIGIT_BIT == 7)
int FFT_MUL_LOWER_CO = 300;
int FFT_MUL_UPPER_CO = 900000;

int FFT_SQR_LOWER_CO = 100;
int FFT_SQR_UPPER_CO = 900000;
#else
#  ifdef _MSC_VER
#    pragma message("7, 15, 28 and 60 bit limbs only (no 31 bit, sorry).")
#  else
#    warning "7, 15, 28 and 60 bit limbs only (no 31 bit, sorry)."
#  endif
#endif /* FFT */




#endif

