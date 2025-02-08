#include "tommath_private.h"
#ifdef MP_GCD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Greatest Common Divisor using the binary method */
mp_err mp_gcd(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int  u, v;
   int     k, u_lsb, v_lsb;
   mp_err err = MP_OKAY;

   /* either zero than gcd is the largest */
   if (mp_iszero(a)) {
      if ((err = mp_abs(b, c)) != MP_OKAY)                              MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }
   if (mp_iszero(b)) {
      if ((err = mp_abs(a, c)) != MP_OKAY)                              MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }

   /* get copies of a and b we can modify */
   if ((err = mp_init_copy(&u, a)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR);
   if ((err = mp_init_copy(&v, b)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_U);


   /* must be positive for the remainder of the algorithm */
   u.sign = v.sign = MP_ZPOS;

   /* B1.  Find the common power of two for u and v */
   u_lsb = mp_cnt_lsb(&u);
   v_lsb = mp_cnt_lsb(&v);
   k     = MP_MIN(u_lsb, v_lsb);

   if (k > 0) {
      /* divide the power of two out */
      if ((err = mp_div_2d(&u, k, &u, NULL)) != MP_OKAY)                 MP_TRACE_ERROR(err, LTM_ERR_V);
      if ((err = mp_div_2d(&v, k, &v, NULL)) != MP_OKAY)                 MP_TRACE_ERROR(err, LTM_ERR_V);
   }

   /* divide any remaining factors of two out */
   if (u_lsb != k) {
      if ((err = mp_div_2d(&u, u_lsb - k, &u, NULL)) != MP_OKAY)         MP_TRACE_ERROR(err, LTM_ERR_V);
   }

   if (v_lsb != k) {
      if ((err = mp_div_2d(&v, v_lsb - k, &v, NULL)) != MP_OKAY)         MP_TRACE_ERROR(err, LTM_ERR_V);
   }

   while (!mp_iszero(&v)) {
      /* make sure v is the largest */
      if (mp_cmp_mag(&u, &v) == MP_GT) {
         /* swap u and v to make sure v is >= u */
         mp_exch(&u, &v);
      }
      /* subtract smallest from largest */
      if ((err = s_mp_sub(&v, &u, &v)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR_V);
      /* Divide out all factors of two */
      if ((err = mp_div_2d(&v, mp_cnt_lsb(&v), &v, NULL)) != MP_OKAY)    MP_TRACE_ERROR(err, LTM_ERR_V);
   }

   /* multiply by 2**k which we divided out at the beginning */
   if ((err = mp_mul_2d(&u, k, c)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR_V);
   c->sign = MP_ZPOS;


LTM_ERR_V:
   mp_clear(&u);
LTM_ERR_U:
   mp_clear(&v);
LTM_ERR:
   return err;
}
#endif
