#include "tommath_private.h"
#ifdef MP_READ_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

static size_t s_mp_strlen(const char *s)
{
   const char *p;
   p = s;
   while (*p != '\0') {
      p++;
   }
   return (size_t)(p - s);
}

/* read a string [ASCII] in a given radix */
mp_err mp_read_radix(mp_int *a, const char *str, int radix)
{

   mp_err   err;
   mp_sign  sign = MP_ZPOS;
   size_t slen;

   /* make sure the radix is ok */
   if ((radix < 2) || (radix > 64)) {
      return MP_VAL;
   }

   /* if the leading digit is a
    * minus set the sign to negative.
    */
   if (*str == '-') {
      ++str;
      sign = MP_NEG;
   }

   slen = s_mp_strlen(str);

   mp_zero(a);

   /* Try faster version first */
   if (MP_HAS(S_MP_FASTER_READ_RADIX)) {
      if ((err = s_mp_faster_read_radix(a, str, 0, slen, radix)) != MP_OKAY)                             goto LTM_ERR;
   } else if (MP_HAS(S_MP_SLOWER_READ_RADIX)) {
      if ((err = s_mp_slower_read_radix(a, str, 0, slen, radix)) != MP_OKAY)                             goto LTM_ERR;
   }

   /* set the sign only if a != 0 */
   if (!mp_iszero(a)) {
      a->sign = sign;
   }

LTM_ERR:
   return err;
}

#endif
