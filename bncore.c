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
int        FFT_MUL_CUTOFF       = 1<<11; // 11
int        FFT_UPPER_LIMIT      = 1<<15; /* less than theoretical limit (20 for 60 and 21 for 28) */
int        FFT_SQR_CUTOFF       = 1<<5;// 2^10

/* 
   Minimum size of the denominator for Burnikel-Ziegler division used
   as a cutoff to school-division inside the algorithm.
   
   The theoretical minimum is 2* KARATS.-CUT. but we have some additional
   overhead, hence the 10 limbs for angst-allowance.
   
   Published for easier changing because: YMMV and it easier this way.
   If somebody has changed this file already: the magic number 48 is the
   value of the KARATSUBA_MUL_CUTOFF of the author's machine.
   Please adjust accordingly
*/
int        DIV_BURN_ZIEG_CUTOFF = 2 * 48  + 10;

/* 
   Minimum sizes of for Burnikel-Ziegler division used
   as a general cutoff to school-division.
      
    Published for easier changing because: YMMV
*/
int        BURN_ZIEG_NUM_CUTOFF =  250;
int        BURN_ZIEG_UPPER_NUM_CUTOFF =  775; 
int        BURN_ZIEG_DEN_CUTOFF =  125;
/* ratio #numerator/#denominator should not be greater than 0.8 */
int        BURN_ZIEG_UPPER_DEN_CUTOFF =  8;
/*
   Size of the denominator chunks inside the Burnikel-Ziegler algorithm.
   
   The actual size gets computed by the algorithm, this is just a start
   value
*/
int        BURN_ZIEG_CHUNK_SIZE =  2 * 48 + 10;

/* Max. size of denominator for Newton division */
int        NEWTON_DEN_CUTOFF =  15000;

/* limit where ilogb wins over native division in mp_radix_size() */
int        RADIX_SIZE_CUTOFF = 110;

#endif

/* $Source$ */
/* $Revision: 0.41 $ */
/* $Date: 2007-04-18 09:58:18 +0000 $ */
