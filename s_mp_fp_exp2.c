#include "tommath_private.h"
#ifdef S_MP_FP_EXP2_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#define MP_FRACBITSQ032 ((int)(sizeof(uint32_t) *(size_t)CHAR_BIT))

static uint32_t s_mp_mulhi_u32(uint32_t a, uint32_t b)
{
   uint32_t r;

   s_mp_32_umul32(a, b, &r, NULL);

   return r;
}

/* computes 2^x - 1 */
static uint32_t s_mp_fp_exp2_fract_new(uint32_t x)
{
   /* Remez coefficients for 2^x - 1 over [0, 1[ scaled to Q0.32 */
   uint32_t r;
   if (x == 0u) {
      return 0u;
   }

   r = 0xe5867;
   r = s_mp_mulhi_u32(r, x) + 0x512969;
   r = s_mp_mulhi_u32(r, x) + 0x27ab6f0;
   r = s_mp_mulhi_u32(r, x) + 0xe33f3a6;
   r = s_mp_mulhi_u32(r, x) + 0x3d7fbfd4;
   r = s_mp_mulhi_u32(r, x) + 0xb17213ac;
   r = s_mp_mulhi_u32(r, x) + 0xb;
   r += 0xFFFFFFFF;
   return r;
}

/* input a is the output from s_mp_fp_log(), a fixed point log base 2 */
mp_err s_mp_fp_exp2(const mp_int *a, mp_int *c)
{
   mp_err err = MP_OKAY;
   mp_int tmp, int_part, fract_part, tmp2;
   uint32_t fraction;
   if ((err = mp_init_multi(&tmp, &int_part, &fract_part, &tmp2, NULL)) != MP_OKAY) {
      return err;
   }
   /* Cut fore and aft to get the integer part */
   if ((err = mp_div_2d(a, MP_FP_SCALE_EXP, &int_part, NULL)) != MP_OKAY)                                 goto LBL_ERR;
   /* Fractional part */
   if ((err = mp_mul_2d(&int_part, MP_FP_SCALE_EXP, &tmp)) != MP_OKAY)                                    goto LBL_ERR;
   if ((err = mp_sub(a, &tmp, &fract_part)) != MP_OKAY)                                                   goto LBL_ERR;

   /* compute 2^fract_part - 1 */
   fraction = s_mp_fp_exp2_fract_new((uint32_t)mp_get_u32(&fract_part));
   /* Add a fixed_point unit to the result */
   if ((err = mp_2expt(&tmp2, 32)) != MP_OKAY)                                                            goto LBL_ERR;
   /* A = 1 << normalized integer part */
   mp_set_u32(&tmp, fraction);
   if ((err = mp_add(&tmp, &tmp2, &tmp2)) != MP_OKAY)                                                     goto LBL_ERR;



   mp_set(&tmp,1);
   if ((err = mp_mul_2d(&tmp, (int)mp_get_l(&int_part), &int_part)) != MP_OKAY)                           goto LBL_ERR;
   if ((err = mp_mul(&int_part, &tmp2, &tmp)) != MP_OKAY)                                                 goto LBL_ERR;
   /* normalize B */
   if ((err = mp_div_2d(&tmp, MP_FP_SCALE_EXP, &tmp, NULL)) != MP_OKAY)                                   goto LBL_ERR;
   /* C = 2^a (Roughly. Very, very roughly) */
   if (c != NULL) {
      mp_exch(&tmp, c);
   }

LBL_ERR:
   mp_clear_multi(&tmp, &int_part, &fract_part,&tmp2, NULL);
   return err;
}
#endif

