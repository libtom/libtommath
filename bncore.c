#include "tommath_private.h"
#ifdef BNCORE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

int KARATSUBA_MUL_CUTOFF = MP_DEFAULT_KARATSUBA_MUL_CUTOFF,
    KARATSUBA_SQR_CUTOFF = MP_DEFAULT_KARATSUBA_SQR_CUTOFF,
    TOOM_MUL_CUTOFF = MP_DEFAULT_TOOM_MUL_CUTOFF,
    TOOM_SQR_CUTOFF = MP_DEFAULT_TOOM_SQR_CUTOFF;

#endif
