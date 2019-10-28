#include "tommath_private.h"
#ifdef MP_CUTOFFS_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef MP_FIXED_CUTOFFS
#include "tommath_cutoffs.h"
size_t MP_KARATSUBA_MUL_CUTOFF = MP_DEFAULT_KARATSUBA_MUL_CUTOFF,
       MP_KARATSUBA_SQR_CUTOFF = MP_DEFAULT_KARATSUBA_SQR_CUTOFF,
       MP_TOOM_MUL_CUTOFF = MP_DEFAULT_TOOM_MUL_CUTOFF,
       MP_TOOM_SQR_CUTOFF = MP_DEFAULT_TOOM_SQR_CUTOFF;
#endif

#endif
