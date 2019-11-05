#include "tommath_private.h"
#ifdef S_MP_SQR_COMBA_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* the jist of squaring...
 * you do like mult except the offset of the tmpx [one that
 * starts closer to zero] can't equal the offset of tmpy.
 * So basically you set up iy like before then you min it with
 * (ty-tx) so that it never happens.  You double all those
 * you add in the inner loop

After that loop you do the squares and add them in.
*/

mp_err s_mp_sqr_comba(const mp_int *a, mp_int *b)
{
   int       oldused, pa, ix;
   mp_word   W1;
   mp_err err;
   mp_int tmp, *b_;

   pa = 2 * a->used;

   /* prepare the destination */
   err = MP_ALIAS(a, b)
         ? mp_init_size((b_ = &tmp), pa)
         : mp_grow((b_ = b), pa);
   if (err != MP_OKAY) {
      return err;
   }

   /* number of output digits to produce */
   W1 = 0;
   for (ix = 0; ix < pa; ix++) {
      int      tx, ty, iy, iz;
      mp_word  W = 0;

      /* get offsets into the two bignums */
      ty = MP_MIN(a->used-1, ix);
      tx = ix - ty;

      /* this is the number of times the loop will iterrate, essentially
         while (tx++ < a->used && ty-- >= 0) { ... }
       */
      iy = MP_MIN(a->used-tx, ty+1);

      /* now for squaring tx can never equal ty
       * we halve the distance since they approach at a rate of 2x
       * and we have to round because odd cases need to be executed
       */
      iy = MP_MIN(iy, ((ty-tx)+1)>>1);

      /* execute loop */
      for (iz = 0; iz < iy; iz++) {
         W += (mp_word)a->dp[tx + iz] * (mp_word)a->dp[ty - iz];
      }

      /* double the inner product and add carry */
      W = W + W + W1;

      /* even columns have the square term in them */
      if (((unsigned)ix & 1u) == 0u) {
         W += (mp_word)a->dp[ix>>1] * (mp_word)a->dp[ix>>1];
      }

      /* store it */
      b_->dp[ix] = (mp_digit)W & MP_MASK;

      /* make next carry */
      W1 = W >> (mp_word)MP_DIGIT_BIT;
   }

   /* setup dest */
   oldused  = b_->used;
   b_->used = 2 * a->used;

   /* clear unused digits [that existed in the old copy of c] */
   s_mp_zero_digs(b_->dp + b_->used, oldused - b_->used);

   mp_clamp(b_);

   if (b_ == &tmp) {
      mp_clear(b);
      *b = *b_;
   }

   return MP_OKAY;
}
#endif
