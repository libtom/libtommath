#include <tommath.h>
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
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */

/* Known optimal configurations

 CPU                    /Compiler     /MUL CUTOFF/SQR CUTOFF
-------------------------------------------------------------
 Intel P4 Northwood     /GCC v3.4.1   /        88/       128/LTM 0.32 ;-)
 AMD Athlon64           /GCC v3.4.4   /        80/       120/LTM 0.35

Curious result:
 AMD Duron (2x64k L1)    /GCC 4.4.5  /         48/       120/LTM 0.42
 
*/

int     KARATSUBA_MUL_CUTOFF = 48;      /* Min. number of digits before Karatsuba multiplication is used. */
int       KARATSUBA_SQR_CUTOFF = 120;     /* Min. number of digits before Karatsuba squaring is used. */      
int        TOOM_MUL_CUTOFF      = 190;      /* no optimal values of these are known yet so set em high */
int        TOOM_SQR_CUTOFF      = 280;
int        TOOM_COOK_4_MUL_CO   = 600; 
int        TOOM_COOK_4_SQR_CO   = 2400;
int        TOOM_COOK_5_MUL_CO   = 1200;
int        TOOM_COOK_5_SQR_CO   = 3600;
        /* No single numbers possible with the current implementation */
int        FFT_MUL_CUTOFF       = 512;
int        FFT_UPPER_LIMIT      = 1<<23;    /* less than actual limit */
int        FFT_SQR_CUTOFF       = 768;

#endif

/* $Source$ */
/* $Revision: 0.41 $ */
/* $Date: 2007-04-18 09:58:18 +0000 $ */
