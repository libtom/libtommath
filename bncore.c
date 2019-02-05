#include "tommath_private.h"
#ifdef BNCORE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * SPDX-License-Identifier: Unlicense
 */

/* Known optimal configurations

 CPU                    /Compiler     /MUL CUTOFF/SQR CUTOFF
-------------------------------------------------------------
 Intel P4 Northwood     /GCC v3.4.1   /        88/       128/LTM 0.32 ;-)
 AMD Athlon64           /GCC v3.4.4   /        80/       120/LTM 0.35

*/

int     KARATSUBA_MUL_CUTOFF = 80,      /* Min. number of digits before Karatsuba multiplication is used. */
        KARATSUBA_SQR_CUTOFF = 120,     /* Min. number of digits before Karatsuba squaring is used. */

        TOOM_MUL_CUTOFF      = 350,      /* no optimal values of these are known yet so set em high */
        TOOM_SQR_CUTOFF      = 400;

#if ((defined LTM_NEED_EXPLICIT_EXPORT) && (defined LTM_EXPORT_CUTOFFS))
int mp_get_KARATSUBA_MUL_CUTOFF(void){
   return KARATSUBA_MUL_CUTOFF;
}
void mp_set_KARATSUBA_MUL_CUTOFF(int cutoff){
   KARATSUBA_MUL_CUTOFF = cutoff;
}
int mp_get_KARATSUBA_SQR_CUTOFF(void){
   return KARATSUBA_SQR_CUTOFF;
}
void mp_set_KARATSUBA_SQR_CUTOFF(int cutoff){
   KARATSUBA_SQR_CUTOFF = cutoff;
}
int mp_get_TOOM_MUL_CUTOFF(void){
   return TOOM_MUL_CUTOFF;
}
void mp_set_TOOM_MUL_CUTOFF(int cutoff){
   TOOM_MUL_CUTOFF = cutoff;
}
int mp_get_TOOM_SQR_CUTOFF(void){
   return TOOM_SQR_CUTOFF;
}
void mp_set_TOOM_SQR_CUTOFF(int cutoff){
   TOOM_SQR_CUTOFF = cutoff;
}
#endif

#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
