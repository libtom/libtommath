#include "tommath_private.h"
#ifdef S_MP_READ_RADIX_POWER_OF_TWO_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* Reads a bignum from an ASCII string in a given radix that must be a power of two. */

/* TODO: Were there general problems with string.h in LTM? Or with the mem*() functions only? */
static size_t s_strlen(const char *s)
{
   const char *p;
   size_t i = 0;
   for (p = s; *p != '\0'; p++, i++) {}
   return i;
}


mp_err s_mp_read_radix_power_of_two(mp_int *a, const char *str, int radix, int radix_bit)
{
   mp_err   err;
   mp_sign  sign = MP_ZPOS;

   /* Length of string w/o sign and NUL */
   size_t slen;
   /* Size of big integer */
   int alen;
   /* Pointer to the end of the string */
   const char *_s;

   /* Count of bits residing in buffer */
   int w_bit = 0;

   int i;
   /* Buffer */
   mp_word W = 0u;

   /* if the leading digit is a
    * minus set the sign to negative.
    */
   if (*str == '-') {
      ++str;
      sign = MP_NEG;
   }

   /*
      We cannot compute the bit-size directly, but for bases of the form 2^n the bit-length
      of each base b digit is an integer so the number of characters in the string
      times log_2(b) is the maximum bit-size of the big integer. Just run along and convert.

      One of the problem of the algorithm "Just run along and convert" is the number of leading
      zeros in the very first base b digit. Starting at the end of the string avoids them.

      To find the end of the string we need to go along it all the way down which is not cheap.
      But it has the advantage that we are able to compute the maximum bit-size of the bignum
      with that information and pre-allocate the memory needed.

      This method will over-allocate if the string has leading zeros, we shall get rid of them
      in advance.
    */

   /* TODO: How likely is it? Not very but it would be a way to DoS this method, so ... */
   while ((*str == '0') && (*(str+1) == '0')) {
      str++;
   }

   slen = s_strlen(str);

   /*
       TODO: size_t might not be bigger than an int save the one bit! Check SIZE_MAX?
             Also: slen * (size_t)radix_bit can overflow, too!

             Standard recommends, that it "should not have an integer conversion rank
             greater than a signed long int"
             (ISO/IEC 9899:2011 7.19 p.4)

             SIZE_MAX has minimum of only 65535 (ibid. 7.20.3)

             On the other side: it can only get bigger than INT_MAX if the type for size_t
             is bigger, too.
    */

   /* Check for overflow */
   if (sizeof(size_t) > sizeof(int)) {
      /* Check if (slen * (size_t)radix_bit) overflows already.
         (Assuming standard complying compiler) */
      if ((slen * (size_t)radix_bit) < slen) {
         return MP_OVF;
      } else if ((slen * (size_t)radix_bit) > (size_t)INT_MAX) {
         return MP_OVF;
      }
   }
   /* Set pointer "_s" to the end of the input string but before the "\0" */
   _s = str + slen - 1;

   /* set the integer to zero */
   mp_zero(a);

   /* We roll it up from the back, so skip allowed trailing characters first. */
   while ((*_s == '\r') || (*_s == '\n')) {
      _s--;
      slen--;
   }

   /* A little shortcut */
   if ((slen == 1) && (*_s == '0')) {
      return MP_OKAY;
   }

   /* We got all necessary information: allocate  */
   alen = (int)(((slen * (size_t)radix_bit) / (size_t) MP_DIGIT_BIT) + 1);
   if ((err = mp_grow(a, alen)) != MP_OKAY) {
      return err;
   }
   a->used = alen;

   /*
      We are still not able to implement the aforementioned "Just run along and convert" algorithm
      because MP_DIGIT_BIT is rarely a multiple of a power of two.
      To make things a bit simpler we use a little buffer that is able to hold radix_bit + MP_DIGIT_BIT.
      Fill in until full, grab MP_DIGIT_BIT bits and fill the limbs until the buffer is empty again.
      Rinse and repeat.

      We have "alen" limbs to fill. Every limb wants MP_DIGIT_BIT except the most significant one
      which might need less.
    */
   for (i=0; slen != 0;) {
      uint8_t y;
      char ch = (radix <= 36) ? (char)MP_TOUPPER((int)*_s) : *_s;
      unsigned pos = (unsigned)(ch - '+');
      if (MP_RADIX_MAP_REVERSE_SIZE <= pos) {
         err = MP_VAL;
         goto LBL_ERR;
      }
      y = s_mp_radix_map_reverse[pos];
      if (y >= radix) {
         err = MP_VAL;
         goto LBL_ERR;
      }
      /* Fill buffer MSB -> LSB */
      W += ((mp_word)y) << w_bit;
      /* Keep bit_count */
      w_bit += radix_bit;
      /* Adjust string pointer and character counter. */
      _s--;
      slen--;

      /* Get them out of the buffer to fill the limbs. */
      if (w_bit >= MP_DIGIT_BIT) {
         a->dp[i++] = (mp_digit)(W & MP_MASK);
         w_bit -= MP_DIGIT_BIT;
         W >>= MP_DIGIT_BIT;
      }
      /* Handle MSB-remainder if any */
      if (slen == 0) {
         a->dp[i] = (mp_digit)(W & MP_MASK);
         break;
      }
   }

   /* TODO: should not be necessary anymore, check */
   mp_clamp(a);
   a->sign = sign;

   return MP_OKAY;
LBL_ERR:
   /* To return zero in case of error seemed sensible at the time of writing. */
   mp_zero(a);
   return err;
}

#endif
