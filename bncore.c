/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is library that provides for multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library is designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@iahu.ca, http://math.libtomcrypt.org
 */
#include <tommath.h>

/* configured for a AMD Duron Morgan core with etc/tune.c */
int     KARATSUBA_MUL_CUTOFF = 73,	/* Min. number of digits before Karatsuba multiplication is used. */
        KARATSUBA_SQR_CUTOFF = 121,	/* Min. number of digits before Karatsuba squaring is used. */
        MONTGOMERY_EXPT_CUTOFF = 128;	/* max. number of digits that montgomery reductions will help for */


