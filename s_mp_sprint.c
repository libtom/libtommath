#include "tommath_private.h"
#ifdef S_MP_SPRINT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



#ifndef MP_NO_FILE
#ifdef MP_WITH_MP_FPRINTF
/* Print a formatted big integer to a string buffer.

   Basically mp_fwrite() with a limit. Here maxlen is the printf "width".
   Use of "precision" does not make much sense, if we don't use scientific
   notation.
   There is a possibility to use a representation of the form
      ddddd"..."dddd
   That is the "width" number of digits and the "precision" number of digits
   at the end visually connected by an ellipsis (three periods).
   Currently only "width" is implemented.

   TODO: According to standard:
      On success, the total number of characters written is returned.
      If a writing error occurs, the error indicator (ferror) is set
         and a negative number is returned.
      If a multibyte character encoding error occurs while writing
         wide characters, errno is set to EILSEQ and a negative
         number is returned.

    TODO: offer a mp_snprintf(), too?
  */
int s_mp_sprint(const mp_int *a, size_t maxlen, unsigned int flags, int radix, char **formatted)
{
   char *buf, *cbuf;
   mp_err err;
   size_t size, written, mlen, extra = 0;
   int r = 0, i;
   bool prefix = false, forced_sign = false, space_sign = false, left_justify = false, zero_pad = false;


   if (flags & FLAG_FORCE_SIGN) {
      forced_sign = true;
      extra++;
   }
   if (flags & FLAG_SPACE_SIGN) {
      space_sign = true;
      extra++;
   }
   if (flags & FLAG_HASH) {
      prefix = true;
      extra +=2;
   }
   if (flags & FLAG_LEFT_JUSTIFY) {
      left_justify = true;
   }
   if (flags & FLAG_ZERO_PAD) {
      zero_pad = true;
   }

   /* We need the exact value here. */
   if ((err = mp_radix_size(a, radix, &size)) != MP_OKAY) {
      return err;
   }
   if (maxlen > 0) {
      mlen = maxlen;
   } else {
      mlen = size;
   }

   /* We have the exact number of digits and the limit "mlen",
      we can measure the padding needed and allocate accordingly */
   if ((mlen > size) && (left_justify || zero_pad)) {
      /* we need padding for the difference between  mlen and size */
      extra += mlen - size;
   }

   buf = (char *) MP_MALLOC(size + extra);
   if (buf == NULL) {
      return MP_MEM;
   }
   /* Why put it in the buffer instead of just printing it? For the s(n)printf() coming next */
   cbuf = buf;

   /* Padding with spaces comes first */
   if (left_justify) {
      for (i = 0; i< (int)(mlen - size); i++) {
         *buf = (char) ' ';
         buf++;
         r++;
      }
   }
   if (space_sign && !mp_isneg(a)) {
      *buf = (char) ' ';
      buf++;
      r++;
   }
   if (forced_sign && !mp_isneg(a)) {
      *buf = (char) '+';
      buf++;
      r++;
   }

   /* Is there a prefix for base64? Would it be useful to invent one?
     (I like "@", it's ASCII and distinct) */
   if (prefix) {
      switch (radix) {
      case 2 :
         *buf = (char) '0';
         buf++;
         r++;
         *buf = (char) 'b';
         buf++;
         r++;
         break;
      case 8 :
         *buf = (char) '0';
         buf++;
         r++;
         break;
      case 16:
         *buf = (char) '0';
         buf++;
         r++;
         *buf = (char) 'x';
         buf++;
         r++;
         break;
      }
   }

   /* Padding with zeros comes after the prefix */
   if (zero_pad) {
      for (i = 0; i< (int)(mlen - size); i++) {
         *buf = (char) '0';
         buf++;
         r++;
      }
   }
   *buf = (char) '\0';

   if ((err = mp_to_radix(a, buf, size, &written, radix)) == MP_OKAY) {
      *formatted = cbuf;
   }

   if (err != MP_OKAY) {
      return -1;
   } else {
      if ((maxlen > 0) && (maxlen < written)) {
         r += (int)maxlen;
      } else {
         r += (int)written;
      }
   }
   return r;
}

#endif
#endif
#endif
