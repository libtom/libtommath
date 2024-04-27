#include "tommath_private.h"
#ifdef S_MP_FASTER_READ_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


mp_err s_mp_faster_read_radix(mp_int *a, const char *str, size_t start, size_t end, int radix)
{
   size_t len, mid;
   mp_int A, B, m;
   mp_digit radix_ = (mp_digit)radix;
   mp_err err = MP_OKAY;

   len = end - start;

   if ((len * (size_t)(s_mp_log2_radix[radix]) + 1) < (size_t)MP_RADIX_READ_CUTOFF) {
      return s_mp_slower_read_radix(a, str, start, end, radix);
   }
   mid = len / 2u;

   if ((err = mp_init_set(&m, radix_)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_init_multi(&A, &B, NULL)) != MP_OKAY) {
      mp_clear(&m);
      return err;
   }

   if ((err = s_mp_faster_read_radix(&A, str, start, start + mid + 1, radix)) != MP_OKAY)                goto LTM_ERR;
   if ((err = s_mp_faster_read_radix(&B, str, start + mid + 1, end, radix)) != MP_OKAY)                  goto LTM_ERR;

   if (MP_IS_2EXPT((unsigned int)radix_)) {
      if ((err = mp_mul_2d(&A, (int)(((len - mid) - 1u) * s_mp_log2_radix[radix_]), &A)) != MP_OKAY)goto LTM_ERR;
   } else {
      if ((err = mp_expt_n(&m, (int)((len - mid) - 1u), &m)) != MP_OKAY)                                 goto LTM_ERR;
      if ((err = mp_mul(&A, &m, &A)) != MP_OKAY)                                                         goto LTM_ERR;
   }
   if ((err = mp_add(&A, &B, a)) != MP_OKAY)                                                             goto LTM_ERR;

LTM_ERR:
   mp_clear_multi(&A, &B, &m, NULL);
   return err;
}


#endif
