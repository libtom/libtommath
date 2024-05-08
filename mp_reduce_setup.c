#include "tommath_private.h"
#ifdef MP_REDUCE_SETUP_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* pre-calculate the value required for Barrett reduction
 * For a given modulus "b" it calculates the value required in "a"
 */
mp_err mp_reduce_setup(mp_int *a, const mp_int *b)
{
   mp_err err = MP_OKAY;
   if ((err = mp_2expt(a, b->used * 2 * MP_DIGIT_BIT)) != MP_OKAY)       MP_TRACE_ERROR(err, LTM_ERR);
   if ((err = mp_div(a, b, a, NULL)) != MP_OKAY)                        MP_TRACE_ERROR(err, LTM_ERR);
LTM_ERR:
   return err;
}
#endif
