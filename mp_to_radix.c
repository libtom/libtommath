#include "tommath_private.h"
#ifdef MP_TO_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* stores a bignum as a ASCII string in a given radix (2..64)
 *
 * Stores upto "size - 1" chars and always a NULL byte, puts the number of characters
 * written, including the '\0', in "written".
 */
mp_err mp_to_radix(const mp_int *a, char *str, size_t maxlen, size_t *written, int radix)
{
   mp_err err;
   mp_int a_bar = *a;
   size_t part_written = 0;

   /* check range of radix and size*/
   if (maxlen < 2u) {
      return MP_BUF;
   }
   if ((radix < 2) || (radix > 64)) {
      return MP_VAL;
   }

   /* quick out if its zero */
   if (mp_iszero(a)) {
      *str++ = '0';
      *str = '\0';
      if (written != NULL) {
         *written = 2u;
      }
      return MP_OKAY;
   }

   /* if it is negative output a - */
   if (mp_isneg(a)) {
      /* store the flag and mark the number as positive */
      *str++ = '-';
      a_bar.sign = MP_ZPOS;

      /* subtract a char */
      --maxlen;
   }

   /* TODO: check if it can be done better */
   if (MP_HAS(S_MP_FASTER_TO_RADIX)) {
      if ((err = s_mp_faster_to_radix(&a_bar, str, maxlen, &part_written, radix)) != MP_OKAY)            goto LBL_ERR;
   } else {
      if (MP_HAS(S_MP_SLOWER_TO_RADIX)) {
         if ((err = s_mp_slower_to_radix(&a_bar, &str, &maxlen, &part_written, radix, false)) != MP_OKAY) goto LBL_ERR;
         /* part_written does not count EOS */
         part_written++;
      }
   }

   /* TODO: Think about adding a function for base-2 radices only although
            s_faster_to_radix is rather quick with such radices. */

   if (written != NULL) {
      part_written += mp_isneg(a) ? 1: 0;
      *written = part_written;
   }

LBL_ERR:
   return err;
}

#endif
