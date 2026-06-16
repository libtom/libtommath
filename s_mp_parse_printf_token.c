#include "tommath_private.h"
#ifdef S_MP_PARSE_PRINTF_TOKEN_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef MP_NO_FILE
#ifdef MP_WITH_MP_FPRINTF
/* Parses a single(!) format specifier */
bool s_mp_parse_printf_token(const char *format, printf_token *token)
{
   bool match;
   /* TODO: Linear search for verification. switch to a switch? */
   /* Extra specifiers for bigint only: 'b', 'B' for binary output
      (also in C23 now) and 'K' (uppercase 'k') for base 64.
    */
   const char *specifiers = "iduopxXcsfFgGeEtpaAnbBK%";
   const char *p = format + 1;
   int i;

   if ((format == NULL) || (*format != '%')) {
      return false;
   }

   token->start_ptr = format;
   token->flags = 0;
   token->width = MP_PRINTF_OMITTED;
   token->precision = MP_PRINTF_OMITTED;
   token->length = LEN_NONE;
   token->specifier = '\0';


   while (*p) {
      if (*p == '-') {
         token->flags |= FLAG_LEFT_JUSTIFY;
      } else if (*p == '+') {
         token->flags |= FLAG_FORCE_SIGN;
      } else if (*p == ' ') {
         token->flags |= FLAG_SPACE_SIGN;
      } else if (*p == '#') {
         token->flags |= FLAG_HASH;
      } else if (*p == '0') {
         token->flags |= FLAG_ZERO_PAD;
      } else {
         break;
      }
      p++;
   }


   if (*p == '*') {
      token->width = MP_PRINTF_DYNAMIC;
      p++;
   } else if (isdigit((unsigned char)*p)) {
      token->width = 0;
      /* TODO: check length and use itoa */
      while (isdigit((unsigned char)*p)) {
         token->width = token->width * 10 + (*p - '0');
         p++;
      }
   }


   if (*p == '.') {
      p++;
      if (*p == '*') {
         token->precision = MP_PRINTF_DYNAMIC;
         p++;
      } else {
         token->precision = 0;
         /* TODO: check length and use itoa? */
         while (isdigit((unsigned char)*p)) {
            token->precision = token->precision * 10 + (*p - '0');
            p++;
         }
      }
   }

   /* The C23 modifiers (wN, wfN) are not implemented. Yet.
      They are the types listed in stdint.h as intN_t
      and uintN_t respectively for "wN" and int_fastN_t
      and uint_fastN_t respectively for "wfN".
      "H", "D", and "DD" for the _DecimalN are not
      implemented. Mainly because compiler support is
      also, let's say, incomplete.
    */
   switch (*p) {
   case 'h':
      if (*(p + 1) == 'h') {
         token->length = LEN_hh;
         p += 2;
      } else                 {
         token->length = LEN_h;
         p++;
      }
      break;
   case'l':
      if (*(p + 1) == 'l') {
         token->length = LEN_ll;
         p += 2;
      } else                 {
         token->length = LEN_l;
         p++;
      }
      break;
   case 'j':
      token->length = LEN_j;
      p++;
      break;
   case 't':
      token->length = LEN_t;
      p++;
      break;
   case 'z':
      token->length = LEN_z;
      p++;
      break;
   case 'L':
      token->length = LEN_L;
      p++;
      break;
   case 'Z':
      token->length = LEN_Z;
      p++;
      break;
   }


   if (*p == '\0') {
      return false;
   }

   match = true;
   /* Only check for validity now, details in the main loop */
   for (i = 0;; i++) {
      if (*p == '\0') {
         match = false;
         break;
      }
      if (*p == specifiers[i]) {
         break;
      }
   }

   if (match) {
      token->specifier = *p;
      p++;
      token->end_ptr = p;
      return true;
   }

   return false;
}


#endif
#endif
#endif
