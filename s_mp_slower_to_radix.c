#include "tommath_private.h"
#ifdef S_MP_SLOWER_TO_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

static void s_reverse(char *s, size_t len)
{
   size_t ix = 0, iy = len - 1u;
   while (ix < iy) {
      MP_EXCH(char, s[ix], s[iy]);
      ++ix;
      --iy;
   }
}

mp_err s_mp_slower_to_radix(const mp_int *a, char **str,
                            size_t *part_maxlen, size_t *part_written, int radix, bool pad)
{
   size_t digs = 0u;
   mp_int t;
   mp_digit d;
   mp_err err = MP_OKAY;

   /* The number of digits of "radix" to be filled if this chunk is not the most significant one. */
   int ybar = s_mp_radix_exponent_y[radix] * MP_RADIX_BARRETT_START_MULTIPLICATOR;

   /* A temporary pointer to the output string to make reversal simpler */
   char *s = *str;

   /* TODO: input a is already a copy of the original and we could use it destructively? */
   if ((err = mp_init_copy(&t, a)) != MP_OKAY)                                                           goto LTM_ERR;

   while (!mp_iszero(&t)) {
      /* TODO: this method to decrease "maxlen" is not threadsafe! */
      if ((--(*part_maxlen)) < 1u) {
         /* no more room */
         err = MP_BUF;
         goto LTM_ERR;
      }
      if ((err = mp_div_d(&t, (mp_digit)radix, &t, &d)) != MP_OKAY) {
         goto LTM_ERR;
      }
      *s++ = s_mp_radix_map[d];
      ++digs;
      ybar--;
   }

   /* Fill in leading zeros if this chunk does not contain the most significant digits. */
   if (pad) {
      while ((ybar-- > 0) && (((*part_maxlen)--) > 0)) {
         *s++ = '0';
         digs++;
      }
   }

   /* TODO: I think that can be done more elegantly */
   /* "rewind" */
   s = *str;
   /* reverse */
   s_reverse(s, digs);
   /* step forward */
   *str += digs;
   /* Add EOS at the end of every chunk to allow this function to be used stand-alone */
   **str = '\0';

   /* TODO: this method to increase "written" is not threadsafe! */
   if (part_written != NULL) {
      *part_written = *part_written + digs;
   }

   err = MP_OKAY;
LTM_ERR:
   mp_clear(&t);
   return err;
}

#endif
