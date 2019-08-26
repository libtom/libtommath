#include "tommath_private.h"
#include <string.h>
#ifdef BN_MP_TODECIMAL_FAST_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* store a bignum as a decimal ASCII string */
mp_err mp_todecimal_fast_rec(mp_int *number, mp_int *nL, mp_int *shiftL, mp_int *mL, int precalc_array_index, int left,
                             char **result)
{
   mp_int q, nLq, r;
   mp_err err;

   if (precalc_array_index < 0) {
      int new_pos = left ? snprintf(*result, 4, "%u", mp_get_i32(number)) : snprintf(*result, 4, "%03u",
                    mp_get_i32(number));
      *result += new_pos;
      return MP_OKAY;
   }

   if ((err = mp_init_multi(&q, &nLq, &r, NULL)) != MP_OKAY) {
      goto LBL_ERR;
   }
   if ((err = mp_mul(number, &mL[precalc_array_index], &q)) != MP_OKAY) {
      goto LBL_ERR;
   }
   if ((err = mp_div_2d(&q, mp_get_i32(&shiftL[precalc_array_index]), &q, NULL)) != MP_OKAY) {
      goto LBL_ERR;
   }

   if ((err = mp_mul(&nL[precalc_array_index], &q, &nLq)) != MP_OKAY) {
      goto LBL_ERR;
   }

   if ((err = mp_sub(number, &nLq, &r)) != MP_OKAY) {
      goto LBL_ERR;
   }

   if (mp_isneg(&r)) {
      if ((err = mp_sub_d(&q, 1, &q)) != MP_OKAY) {
         goto LBL_ERR;
      }
      if ((err = mp_add(&r, &nL[precalc_array_index], &r)) != MP_OKAY) {
         goto LBL_ERR;
      }
   }

   --precalc_array_index;
   if (left && mp_iszero(&q)) {
      if ((err = mp_todecimal_fast_rec(&r, nL, shiftL, mL, precalc_array_index, 1, result)) != MP_OKAY) {
         goto LBL_ERR;
      }
   } else {
      if ((err = mp_todecimal_fast_rec(&q, nL, shiftL, mL, precalc_array_index, left, result)) != MP_OKAY) {
         goto LBL_ERR;
      }
      if ((err = mp_todecimal_fast_rec(&r, nL, shiftL, mL, precalc_array_index, 0, result)) != MP_OKAY) {
         goto LBL_ERR;
      }
   }

   err = MP_OKAY;

LBL_ERR:
   mp_clear_multi(&q, &nLq, &r, NULL);
   return err;
}

mp_err mp_todecimal_fast(mp_int *number, char *result)
{
   mp_int n, shift, M, M2, M22, M4, M44;
   mp_int nL[20], shiftL[20], mL[20];
   mp_err err;
   char **result_addr = &result;
   int precalc_array_index = 1;

   if ((err = mp_init_multi(&M2, &M22, &M4, &M44, NULL)) != MP_OKAY) {
      goto LBL_ERR;
   }

   if (mp_isneg(number)) {
      if ((err = mp_neg(number, number)) != MP_OKAY) {
         goto LBL_ERR;
      }
      result[0] = '-';
      result_addr += 1;
   }
   if ((err = mp_init_set(&n, (mp_digit)1000)) != MP_OKAY) {
      goto LBL_ERR;
   }

   if ((err = mp_init_copy(&nL[0], &n)) != MP_OKAY) {
      goto LBL_ERR;
   }

   if ((err = mp_init_set(&shift, (mp_digit)20)) != MP_OKAY) {
      goto LBL_ERR;
   }

   if ((err = mp_init_copy(&shiftL[0], &shift)) != MP_OKAY) {
      goto LBL_ERR;
   }

   /* (8 * 2**$shift) / $n rounded up */
   if ((err = mp_init_set(&M, (mp_digit)8389)) != MP_OKAY) {
      goto LBL_ERR;
   }

   /* $M / 8, rounded up */
   if ((err = mp_init_set(&mL[0], (mp_digit)1049)) != MP_OKAY) {
      goto LBL_ERR;
   }

   while (1) {
      if ((err = mp_sqr(&n, &n)) != MP_OKAY) {
         goto LBL_ERR;
      }
      if (mp_cmp(&n, number) == MP_GT) {
         break;
      }

      if ((err = mp_mul_2(&shift, &shift)) != MP_OKAY) {
         goto LBL_ERR;
      }

      /* The following is a Newton-Raphson step, to restore the invariant
       * that $M is (8 * 2**$shift) / $n, rounded up. */
      {
         if ((err = mp_sqr(&M, &M2)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_sqr(&M2, &M4)) != MP_OKAY) {
            goto LBL_ERR;
         }

         if ((err = mp_mul(&M4, &n, &M4)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_div_2d(&M4, mp_get_i32(&shift) + 6, &M4, NULL)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_mul_2(&M2, &M2)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_sub(&M4, &M2, &M4)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_add_d(&M4, 1, &M4)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_div_2d(&M4, 3, &M4, NULL)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_sub_d(&M4, 1, &M4)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_neg(&M4, &M)) != MP_OKAY) {
            goto LBL_ERR;
         }
      }

      if ((err = mp_init_copy(&nL[precalc_array_index], &n)) != MP_OKAY) {
         goto LBL_ERR;
      }
      if ((err = mp_init_copy(&shiftL[precalc_array_index], &shift)) != MP_OKAY) {
         goto LBL_ERR;
      }

      /* Divide by 8, round up */
      {
         if ((err = mp_add_d(&M4, 1, &M4)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_div_2d(&M4, 3, &M4, NULL)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_sub_d(&M4, 1, &M4)) != MP_OKAY) {
            goto LBL_ERR;
         }
         if ((err = mp_neg(&M4, &M4)) != MP_OKAY) {
            goto LBL_ERR;
         }
      }
      if ((err = mp_init_copy(&mL[precalc_array_index], &M4)) != MP_OKAY) {
         goto LBL_ERR;
      }
      precalc_array_index++;
   }

   if ((err = mp_todecimal_fast_rec(number, nL, shiftL, mL, precalc_array_index - 1, 1, result_addr)) != MP_OKAY) {
      goto LBL_ERR;
   }

   err = MP_OKAY;

LBL_ERR:
   mp_clear_multi(&n, &shift, &M, &M2, &M22, &M4, &M44, NULL);
   return err;
}

#endif
