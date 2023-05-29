#include "tommath_private.h"
#ifdef S_MP_PRINT_MP_DIGIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



static const char s_mp_base64[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
/* Maximal sizes for mp_digit (come in three flavours) in bases 2-64 */
#if (MP_DIGIT_BIT > 32)
static const uint8_t s_mp_digit_length[65] = {
   0, 0, 63, 40, 31, 27, 24, 22, 21, 20, 19, 18, 17,
   17, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13,
   13, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12, 12, 12,
   12, 12, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
   11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 10, 10, 10
};
#elif ((MP_DIGIT_BIT < 32) &&  (MP_DIGIT_BIT > 15))
static const uint8_t s_mp_digit_length[65] = {
   0, 0, 31, 20, 15, 13, 12, 11, 10, 10, 9, 9, 8, 8,
   8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6,
   6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5,
   5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};
#else
static const uint8_t s_mp_digit_length[65] = {
   0, 0, 15, 10, 7, 6, 6, 5, 5, 5, 4, 4, 4, 4, 4, 4,
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
   3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};
#endif
/* TODO: thousands separators (needs locale environment, so prob. not) */
int s_mp_print_mp_digit(mp_digit number, mp_digit base, bool fill, char *buffer)
{
   mp_digit cp, digit;
   int i = 0;

   if (number == 0u) {
      buffer[0] = '0';
   }

   cp = number;

   while (cp != 0) {
      digit = cp % base;
      cp /= base;
      buffer[i++] = s_mp_base64[digit];
   }
   /* It is easier to do it here instead of the main function for printing the array */
   if (fill) {
      while (i < (int)s_mp_digit_length[base]) {
         buffer[i++] = '0';
      }
   }
   s_mp_str_reverse(buffer, i);
   return i;
}



#endif
