#include "tommath_private.h"
#ifdef S_MP_MUL_HIGH_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* multiplies |a| * |b| and does not compute the lower digs digits
 * [meant to get the higher part of the product]
 */
mp_err s_mp_mul_high(const mp_int *a, const mp_int *b, mp_int *c, int digs)
{
   mp_int   tmp, *c_;
   int      pa, pb, ix;
   mp_err   err;

   /* prepare the destination */
   err = (MP_ALIAS(a, c) || MP_ALIAS(b, c))
         ? mp_init_size((c_ = &tmp), a->used + b->used + 1)
         : mp_grow((c_ = c), a->used + b->used + 1);
   if (err != MP_OKAY) {
      return err;
   }

   s_mp_zero_digs(c_->dp, c_->used);
   c_->used = a->used + b->used + 1;

   pa = a->used;
   pb = b->used;
   for (ix = 0; ix < pa; ix++) {
      int iy;
      mp_digit u = 0;

      for (iy = digs - ix; iy < pb; iy++) {
         /* calculate the double precision result */
         mp_word r = (mp_word)c_->dp[ix + iy] +
                     ((mp_word)a->dp[ix] * (mp_word)b->dp[iy]) +
                     (mp_word)u;

         /* get the lower part */
         c_->dp[ix + iy] = (mp_digit)(r & (mp_word)MP_MASK);

         /* carry the carry */
         u       = (mp_digit)(r >> (mp_word)MP_DIGIT_BIT);
      }
      c_->dp[ix + pb] = u;
   }

   mp_clamp(c_);

   if (c_ == &tmp) {
      mp_clear(c);
      *c = *c_;
   }

   return MP_OKAY;
}
#endif
