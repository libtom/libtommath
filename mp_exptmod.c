#include "tommath_private.h"
#ifdef MP_EXPTMOD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* this is a shell function that calls either the normal or Montgomery
 * exptmod functions.  Originally the call to the montgomery code was
 * embedded in the normal function but that wasted a lot of stack space
 * for nothing (since 99% of the time the Montgomery code would be called)
 */
mp_err mp_exptmod(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y)
{
   int dr;
   mp_err err = MP_OKAY;

   /* modulus P must be positive */
   if (mp_isneg(P)) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR_END);
   }

   /* if exponent X is negative we have to recurse */
   if (mp_isneg(X)) {
      mp_int tmpG, tmpX;

      if (!MP_HAS(MP_INVMOD)) {
         err = MP_VAL;
         MP_TRACE_ERROR(err, LTM_ERR);
      }

      if ((err = mp_init_multi(&tmpG, &tmpX, NULL)) != MP_OKAY)          MP_TRACE_ERROR(err, LTM_ERR);

      /* first compute 1/G mod P */
      if ((err = mp_invmod(G, P, &tmpG)) != MP_OKAY)                     MP_TRACE_ERROR(err, LTM_ERR);

      /* now get |X| */
      if ((err = mp_abs(X, &tmpX)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR);

      /* and now compute (1/G)**|X| instead of G**X [X < 0] */
      if ((err = mp_exptmod(&tmpG, &tmpX, P, Y)) != MP_OKAY)            MP_TRACE_ERROR(err, LTM_ERR);
LTM_ERR:
      mp_clear_multi(&tmpG, &tmpX, NULL);
      return err;
   }

   /* modified diminished radix reduction */
   if (MP_HAS(MP_REDUCE_IS_2K_L) && MP_HAS(MP_REDUCE_2K_L) && MP_HAS(S_MP_EXPTMOD) &&
       mp_reduce_is_2k_l(P)) {
      if ((err = s_mp_exptmod(G, X, P, Y, 1)) != MP_OKAY)               MP_TRACE_ERROR(err, LTM_ERR_END);
      return err;
   }

   /* is it a DR modulus? default to no */
   dr = (MP_HAS(MP_DR_IS_MODULUS) && mp_dr_is_modulus(P)) ? 1 : 0;

   /* if not, is it a unrestricted DR modulus? */
   if (MP_HAS(MP_REDUCE_IS_2K) && (dr == 0)) {
      dr = (mp_reduce_is_2k(P)) ? 2 : 0;
   }

   /* if the modulus is odd or dr != 0 use the montgomery method */
   if (MP_HAS(S_MP_EXPTMOD_FAST) && (mp_isodd(P) || (dr != 0))) {
      if ((err = s_mp_exptmod_fast(G, X, P, Y, dr)) != MP_OKAY)         MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }

   /* otherwise use the generic Barrett reduction technique */
   if (MP_HAS(S_MP_EXPTMOD)) {
      if ((err = s_mp_exptmod(G, X, P, Y, 0)) != MP_OKAY)               MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }

   /* no exptmod for evens */
   err = MP_VAL;

LTM_ERR_END:
   return err;
}

#endif
