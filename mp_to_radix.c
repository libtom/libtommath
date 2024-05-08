#include "tommath_private.h"
#ifdef MP_TO_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* reverse an array, used for radix code */
static void s_reverse(char *s, size_t len)
{
   size_t ix = 0, iy = len - 1u;
   while (ix < iy) {
      MP_EXCH(char, s[ix], s[iy]);
      ++ix;
      --iy;
   }
}

/* stores a bignum as a ASCII string in a given radix (2..64)
 *
 * Stores upto "size - 1" chars and always a NULL byte, puts the number of characters
 * written, including the '\0', in "written".
 */
mp_err mp_to_radix(const mp_int *a, char *str, size_t maxlen, size_t *written, int radix)
{
   size_t  digs;
   mp_err  err = MP_OKAY;
   mp_int  t;
   mp_digit d;
   char   *_s = str;

   /* check range of radix and size*/
   if (maxlen < 2u) {
      err =  MP_BUF;
      MP_TRACE_ERROR(err, LTM_ERR);
   }
   if ((radix < 2) || (radix > 64)) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* quick out if its zero */
   if (mp_iszero(a)) {
      *str++ = '0';
      *str = '\0';
      if (written != NULL) {
         *written = 2u;
      }
      goto LTM_ERR;
   }

   if ((err = mp_init_copy(&t, a)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR);

   /* if it is negative output a - */
   if (mp_isneg(&t)) {
      /* we have to reverse our digits later... but not the - sign!! */
      ++_s;

      /* store the flag and mark the number as positive */
      *str++ = '-';
      t.sign = MP_ZPOS;

      /* subtract a char */
      --maxlen;
   }
   digs = 0u;
   while (!mp_iszero(&t)) {
      if (--maxlen < 1u) {
         /* no more room */
         err = MP_BUF;
         MP_TRACE_ERROR(err, LTM_ERR_1);
      }
      if ((err = mp_div_d(&t, (mp_digit)radix, &t, &d)) != MP_OKAY)      MP_TRACE_ERROR(err, LTM_ERR_1);
      *str++ = s_mp_radix_map[d];
      ++digs;
   }
   /* reverse the digits of the string.  In this case _s points
    * to the first digit [excluding the sign] of the number
    */
   s_reverse(_s, digs);

   /* append a NULL so the string is properly terminated */
   *str = '\0';
   digs++;

   if (written != NULL) {
      *written = mp_isneg(a) ? (digs + 1u): digs;
   }

LTM_ERR_1:
   mp_clear(&t);
LTM_ERR:
   return err;
}

#endif
