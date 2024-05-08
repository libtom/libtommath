#include "tommath_private.h"
#ifdef MP_PRIME_MILLER_RABIN_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Miller-Rabin test of "a" to the base of "b" as described in
 * HAC pp. 139 Algorithm 4.24
 *
 * Sets result to 0 if definitely composite or 1 if probably prime.
 * Randomly the chance of error is no more than 1/4 and often
 * very much lower.
 */
mp_err mp_prime_miller_rabin(const mp_int *a, const mp_int *b, bool *result)
{
   mp_int  n1, y, r;
   mp_err  err = MP_OKAY;
   int     s, j;

   /* ensure b > 1 */
   if (mp_cmp_d(b, 1uL) != MP_GT) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* get n1 = a - 1 */
   if ((err = mp_init_copy(&n1, a)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR);
   if ((err = mp_sub_d(&n1, 1uL, &n1)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR_1);

   /* set 2**s * r = n1 */
   if ((err = mp_init_copy(&r, &n1)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_1);

   /* count the number of least significant bits
    * which are zero
    */
   s = mp_cnt_lsb(&r);

   /* now divide n - 1 by 2**s */
   if ((err = mp_div_2d(&r, s, &r, NULL)) != MP_OKAY)                    MP_TRACE_ERROR(err, LTM_ERR_2);

   /* compute y = b**r mod a */
   if ((err = mp_init(&y)) != MP_OKAY)                                   MP_TRACE_ERROR(err, LTM_ERR_2);
   if ((err = mp_exptmod(b, &r, a, &y)) != MP_OKAY)                      MP_TRACE_ERROR(err, LTM_ERR_END);

   /* if y != 1 and y != n1 do */
   if ((mp_cmp_d(&y, 1uL) != MP_EQ) && (mp_cmp(&y, &n1) != MP_EQ)) {
      j = 1;
      /* while j <= s-1 and y != n1 */
      while ((j <= (s - 1)) && (mp_cmp(&y, &n1) != MP_EQ)) {
         if ((err = mp_sqrmod(&y, a, &y)) != MP_OKAY)                    MP_TRACE_ERROR(err, LTM_ERR_END);

         /* if y == 1 then composite */
         if (mp_cmp_d(&y, 1uL) == MP_EQ) {
            *result = false;
            goto LTM_ERR_END;
         }

         ++j;
      }

      /* if y != n1 then composite */
      if (mp_cmp(&y, &n1) != MP_EQ) {
         *result = false;
         goto LTM_ERR_END;
      }
   }

   /* probably prime now */
   *result = true;

LTM_ERR_END:
   mp_clear(&y);
LTM_ERR_2:
   mp_clear(&r);
LTM_ERR_1:
   mp_clear(&n1);
LTM_ERR:
   return err;
}
#endif
