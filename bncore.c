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
#endif

