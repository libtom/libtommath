/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
/*
   Current values evaluated on an AMD A8-6600K (64-bit).
   Type "make tune" to optimize them for your machine but
   be aware that it may take a long time. It took 2:30 minutes
   on the aforementioned machine for example.
 */
#define MP_DEFAULT_MUL_KARATSUBA_CUTOFF 116
#define MP_DEFAULT_SQR_KARATSUBA_CUTOFF 160
#define MP_DEFAULT_MUL_TOOM_CUTOFF      139
#define MP_DEFAULT_SQR_TOOM_CUTOFF      193
#define MP_DEFAULT_RADIX_READ_CUTOFF    3000
#define MP_DEFAULT_RADIX_WRITE_CUTOFF   480
