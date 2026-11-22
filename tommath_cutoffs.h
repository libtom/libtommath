/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
/*
   Type "make tune" to optimize them for your machine but
   be aware that it will take some time.
 */
#define MP_DEFAULT_MUL_KARATSUBA_CUTOFF 123
#define MP_DEFAULT_SQR_KARATSUBA_CUTOFF 183
#define MP_DEFAULT_MUL_TOOM_CUTOFF      201
#define MP_DEFAULT_SQR_TOOM_CUTOFF      222
#define MP_DEFAULT_MUL_TOOM_4_CUTOFF    412
#define MP_DEFAULT_SQR_TOOM_4_CUTOFF    349
#define MP_DEFAULT_MUL_TOOM_5_CUTOFF    634
#define MP_DEFAULT_SQR_TOOM_5_CUTOFF    356
#define MP_DEFAULT_MUL_TOOM_6_CUTOFF    764
#define MP_DEFAULT_SQR_TOOM_6_CUTOFF    1746
#define MP_DEFAULT_MUL_TOOM_7_CUTOFF    669
#define MP_DEFAULT_SQR_TOOM_7_CUTOFF    1525
