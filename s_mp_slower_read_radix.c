#include "tommath_private.h"
#ifdef S_MP_SLOWER_READ_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err s_mp_slower_read_radix(mp_int *a, const char *str, size_t start, size_t end, int radix)
{
   mp_err err;
   size_t i;

   /* checks are done by caller */

   char *_s = (char *)(str + start);
   for (i = start; (i < end) && (*_s != '\0'); i++) {
      uint8_t y;

      char ch = (radix <= 36) ? (char)MP_TOUPPER((int)*_s) : *_s;
      unsigned int pos = (unsigned int)(ch - '+');
      if (MP_RADIX_MAP_REVERSE_SIZE <= pos) {
         err = MP_VAL;
         goto LBL_ERR;
      }
      y = s_mp_radix_map_reverse[pos];
      if (y >= radix) {
         err = MP_VAL;
         goto LBL_ERR;
      }
      if ((err = mp_mul_d(a, (mp_digit)radix, a)) != MP_OKAY)                                            goto LBL_ERR;
      if ((err = mp_add_d(a, (mp_digit)y, a)) != MP_OKAY)                                                goto LBL_ERR;
      _s++;
   }

   return MP_OKAY;
LBL_ERR:
   mp_zero(a);
   return err;
}

#endif
