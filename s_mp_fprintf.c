#include "tommath_private.h"
#ifdef S_MP_FPRINTF_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef MP_NO_FILE
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>


#ifdef _MSC_VER
/* Ignore warning: "'fprintf' : format string expected in argument 2 is not a string"
   because that's how this function works in the first place */
#pragma warning(disable: 4774)
#endif


/* Step to the next character, throw error if there is none  */
#define LTM_NEXT_ERROR(x) do{(x)++;if(*(x) == '\0'){goto LTM_ERR;}}while(0)
/* Step to the next character, got to end if there is none  */
#define LTM_NEXT(x) do{(x)++;if(*(x) == '\0')break;}while(0)

/* The size of the string "%..." is not very large */
#define LTM_PRINTF_VARIABLE_BUFSIZ 32

static void s_mp_str_reverse(char *s, int len)
{
   int x = 0, y = len - 1;
   char t;

   while (x < y) {
      t = s[x];
      s[x] = s[y];
      s[y] = t;
      x++;
      y--;
   }
}

static int s_mp_print_mp_digit_binary(mp_digit number, bool prefix, char *buffer)
{
   mp_digit cp, digit;
   int i = 0;

   cp = number;
   while (cp != 0) {
      digit = cp % 2;
      cp /= 2;
      buffer[i++] = (digit == 1) ? '1' : '0';
   }

   if (prefix) {
      buffer[i++] = 'b';
      buffer[i++] = '0';
   }

   s_mp_str_reverse(buffer, i);
   return i;
}

static int s_mp_print_bigint(const mp_int *N, const char *format, int base, bool prefix, FILE *stream)
{
   mp_err err;
   char *buf, *start_buf;
   size_t size, written;
   int chars_printed = 0;
   mp_sign sign = MP_ZPOS;
   mp_int _N;

   if ((err = mp_radix_size_overestimate(N, base, &size)) != MP_OKAY) {
      return -1;
   }

   if (prefix) {
      size += 3;
   }

   if ((err = mp_init_copy(&_N, N)) != MP_OKAY)  {
      return -1;
   }

   if (mp_isneg(N)) {
      _N.sign = MP_ZPOS;
      sign = MP_NEG;
   }

   buf = MP_MALLOC(size);
   if (buf == NULL) {
      goto LTM_ERR;
   }

   start_buf = buf;

   if (sign == MP_NEG) {
      *buf++ = '-';
   }

   /* TODO: upper case/lower case? But that means a rewrite of mp_to_radix() */
   if (prefix) {
      *buf++ = '0';
      switch (base) {
      case 2:
         *buf++ = 'b';
         break;
      case 8:
         *buf++ = 'o';
         break;
      case 16:
         *buf++ = 'x';
         break;
      }
   }

   if ((err = mp_to_radix(&_N, buf, size, &written, base)) == MP_OKAY) {
      written--;
   } else {
      MP_FREE(start_buf, size);
      goto LTM_ERR;
   }

   chars_printed = fprintf(stream, format, start_buf);

   MP_FREE(start_buf, size);
   mp_clear(&_N);
   return chars_printed;
LTM_ERR:
   mp_clear(&_N);
   return -1;
}

/* Largest mp_digit has 64 bit but only 60 are used, so 65 seem to be a safe value here. */
#define LTM_BASE2_BUFSIZ 65
static int s_mp_print_limb(const mp_digit d, const char *format, int base, bool prefix, FILE *stream)
{
   char format_buf[LTM_PRINTF_VARIABLE_BUFSIZ] = {0};
   char base2_buffer[LTM_BASE2_BUFSIZ] = {0};
   char *f;
   size_t slen;

   f = &format_buf[0];

   /* assuming conversion character to be the last */
   slen = strlen(format);

   /* strlen("llu") = 3 plus the trailing '\0' makes four  */
   if (slen >= (LTM_PRINTF_VARIABLE_BUFSIZ - 5)) {
      return -1;
   }
   /* For some reason clang complains when strncat is used */
   f = strcat(f, format);
   f[slen-1] = '\0';

   if (base == 2) {
      /* length not used here, fprintf(3) takes care of that */
      (void)s_mp_print_mp_digit_binary(d, prefix, &base2_buffer[0]);
      f = strcat(f, "s");
      return fprintf(stream, f, base2_buffer);
   } else {
#if defined(MP_16BIT)
      switch (base) {
      case 8:
         f = strcat(f, PRIo16);
         break;
      case 10:
         f = strcat(f, PRIu16);
         break;
      case 16:
         f = strcat(f, PRIx16);
         break;
      }
#elif defined(MP_64BIT)
      switch (base) {
      case 8:
         /*
         its defined in the local inttype.h as
# if __WORDSIZE == 64
#  define __PRI64_PREFIX        "l"
#  define __PRIPTR_PREFIX       "l"
# else
#  define __PRI64_PREFIX        "ll"
#  define __PRIPTR_PREFIX
# endif

            ...

# define PRIo64 __PRI64_PREFIX "o"

         */
         f = strcat(f, PRIo64);
         break;
      case 10:
         f = strcat(f, PRIu64);
         break;
      case 16:
         f = strcat(f, PRIx64);
         break;
      }
#else
      switch (base) {
      case 8:
         f = strcat(f, PRIo32);
         break;
      case 10:
         f = strcat(f, PRIu32);
         break;
      case 16:
         f = strcat(f, PRIx32);
         break;
      }
#endif
   }
   return fprintf(stream, f, d);
}


/* format is for the individual limb  */
static int s_mp_print_limb_array(mp_int *n, const char *format, int base, bool prefix, FILE *stream)
{
   int i;
   int chars_printed = 0;

   for (i = 0; i < n->used; i++) {
      chars_printed += s_mp_print_limb(n->dp[i], format, base, prefix, stream);
      if (i < (n->used - 1)) {
         fputc(',', stream);
         chars_printed++;
      }
   }

   return chars_printed;
}

int s_mp_fprintf(FILE *stream, const char *format, va_list args)
{
   int printed_characters = 0;
   /* TODO: enum? preprocessor?*/
   bool is_extra_short = false;
   bool is_short       = false;
   bool is_long        = false;
   bool is_long_long   = false;

   int modifier = 0;
   bool print_prefix = false;
   int base;

   char format_out[LTM_PRINTF_VARIABLE_BUFSIZ] = {0};
   int format_out_index = 0;
   const char *conversion_modifiers = "aAbcdeEfFgGimnopsuxX@";

   /* Assuming ASCII input and a properly terminated string at "format" */
   while (*format != '\0') {
      if (*format == '%') {
         /* TODO: use s_mp_zero_buf(void *mem, size_t size) in production */
         s_mp_zero_buf(format_out, (size_t)LTM_PRINTF_VARIABLE_BUFSIZ);
         format_out_index = 0;
         modifier = 0;
         is_extra_short = false;
         is_short       = false;
         is_long        = false;
         is_long_long   = false;
         print_prefix   = false;

         format_out[format_out_index++] = *format;
         /* Probably the most infamous last words: "Can't happen!" */
         if (format_out_index >= LTM_PRINTF_VARIABLE_BUFSIZ) {
            return -1;
         }
         LTM_NEXT_ERROR(format);
         /* Keeping it simple */
         if (*format == '%') {
            fputc('%', stream);
            printed_characters++;
            break;
         }
         while (strchr(conversion_modifiers, *format) == NULL) {
            switch (*format) {
            case 'h':
               if (!is_short) {
                  is_short = true;
               } else {
                  is_short = false;
                  is_extra_short = true;
               }
               modifier = 'h';
               break;
            case 'l':
               if (!is_long) {
                  is_long = true;
               } else {
                  is_long = false;
                  is_long_long = true;
               }
               modifier = 'l';
               break;
            case '#':
               print_prefix = true;
               break;
            case 'j':
            case 't':
            case 'z':
            case 'Z':
            case 'N':
            case 'M':
               modifier = *format;
               break;
            }
            format_out[format_out_index++] = *format;
            if (format_out_index >= LTM_PRINTF_VARIABLE_BUFSIZ) {
               return -1;
            }
            LTM_NEXT_ERROR(format);
         }

         if (strchr(conversion_modifiers, *format) != NULL) {
            format_out[format_out_index++] = *format;
            if (format_out_index >= LTM_PRINTF_VARIABLE_BUFSIZ) {
               return -1;
            }
            switch (*format) {
               /* TODO: Ii _is_ expensive without hardware support. Offer manual switch-off? */
#ifdef LTM_HAVE_FLOAT_TYPE
            case 'a':
            case 'A':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
            case 'e':
            case 'E':
#ifndef LTM_HAVE_DOUBLE_TYPE
               printed_characters += fprintf(stream, format_out, va_arg(args, float));
#else
               if (modifier == 'L') {
                  /* TODO: make a note in the docs! */
#ifdef LTM_HAVE_LONG_DOUBLE_TYPE
                  printed_characters += fprintf(stream, format_out, va_arg(args, long double));
#elif (defined LTM_HAVE_DOUBLE_TYPE)
                  printed_characters += fprintf(stream, format_out, va_arg(args, double));
#else
                  printed_characters += fprintf(stream, format_out, va_arg(args, float));
#endif /* LTM_HAVE_LONG_DOUBLE_TYPE */
               } else {
#ifdef LTM_HAVE_DOUBLE_TYPE
                  printed_characters += fprintf(stream, format_out, va_arg(args, double));
#else
                  printed_characters += fprintf(stream, format_out, va_arg(args, float));
#endif /* LTM_HAVE_DOUBLE_TYPE */
               }
#endif /* LTM_HAVE_DOUBLE_TYPE */
               break;
#endif /* LTM_HAVE_FLOAT_TYPE */
            case 'd':
            case 'i':
               if (modifier != 0) {
                  switch (modifier) {
                  case 'h':
                     if (is_extra_short) {
                        printed_characters += fprintf(stream, format_out, (signed char)va_arg(args, int));
                     } else {
                        printed_characters += fprintf(stream, format_out, (short)va_arg(args, int));
                     }
                     break;
                  case 'l':
                     if (is_long_long) {
                        printed_characters += fprintf(stream, format_out, va_arg(args, long long));
                     } else {
                        printed_characters += fprintf(stream, format_out, va_arg(args, long));
                     }
                     break;
                  case 'j':
                     printed_characters += fprintf(stream, format_out, va_arg(args,intmax_t));
                     break;
                  case 't':
                     printed_characters += fprintf(stream, format_out, va_arg(args, ptrdiff_t));
                     break;
                  /* Beware: 'Z' (ASCII 0x5a) is an old version of 'z' (ASCII 0x7a)! */
                  case 'Z':
                     format_out[format_out_index - 2] = 's';
                     format_out[format_out_index - 1] = '\0';
                     printed_characters += s_mp_print_bigint(va_arg(args, mp_int *), format_out, 10, false, stream);
                     break;
                  /* It seems a bit brutal, but the limbs _are_ unsigned. */
                  case 'M':
                  case 'N':
                     return -1;
                  }
               } else {
                  printed_characters += fprintf(stream, format_out, va_arg(args, int));
               }
               break;
            case 'B':
            case 'b':
               if (modifier == 'Z') {
                  format_out[format_out_index - 2] = 's';
                  format_out[format_out_index - 1] = '\0';
                  printed_characters += s_mp_print_bigint(va_arg(args, mp_int *), format_out, 2, print_prefix, stream);
               } else if (modifier == 'M') {
                  format_out[format_out_index - 2] = *format;
                  format_out[format_out_index - 1] = '\0';
#ifdef MP_16BIT
                  printed_characters += s_mp_print_limb((mp_digit)va_arg(args, unsigned int), format_out, 2, print_prefix, stream);
#else
                  printed_characters += s_mp_print_limb(va_arg(args, mp_digit), format_out, 2, print_prefix, stream);
#endif
               } else if (modifier == 'N') {
                  format_out[format_out_index - 2] = *format;
                  format_out[format_out_index - 1] = '\0';
                  printed_characters += s_mp_print_limb_array(va_arg(args, mp_int *), format_out, 2, print_prefix, stream);
               } else {
                  /*
                     For big integer related numbers only, but feel free to add the rest,
                     it is not _that_ complicated, just tedious.
                     Use a bigint in the meantime, or a mp_digit ("%Mb") if its size and unsignedness
                     is not a problem.
                   */
                  /* s_mp_print_integer_binary(, MP_SIGNED, print_prefix, uppercase, &buffer); */
                  return -1;
               }
               break;
            case 'o':
            case 'u':
            case 'x':
            case 'X':
               if (modifier != 0) {
                  if (*format == 'o') {
                     base = 8;
                  } else if ((*format == 'x') || (*format == 'X')) {
                     base = 16;
                  } else {
                     base = 10;
                  }
                  /* TODO: this is basically a copy of the signed version */
                  switch (modifier) {
                  case 'h':
                     if (is_extra_short) {
                        printed_characters += fprintf(stream, format_out, (unsigned char)va_arg(args, int));
                     } else {
                        printed_characters += fprintf(stream, format_out, (unsigned short)va_arg(args, int));
                     }
                     break;
                  case 'l':
                     if (is_long_long) {
                        printed_characters += fprintf(stream, format_out, va_arg(args, unsigned long long));
                     } else {
                        printed_characters += fprintf(stream, format_out, va_arg(args, unsigned long));
                     }
                     break;
                  case 'z':
                     printed_characters += fprintf(stream, format_out, va_arg(args, size_t));
                     break;
                  case 'j':
                     printed_characters += fprintf(stream, format_out, va_arg(args, uintmax_t));
                     break;
                  /** If you actually have an unsigned version: please send the authors a short note
                  case 't': printed_characters += fprintf(stream, format_out, va_arg(args, ptrdiff_t));
                            break;
                  */
                  case 'Z':
                     format_out[format_out_index - 2] = 's';
                     format_out[format_out_index - 1] = '\0';
                     printed_characters += s_mp_print_bigint(va_arg(args, mp_int *), format_out, base, print_prefix, stream);
                     break;
                  case 'M':
                     format_out[format_out_index - 2] = *format;
                     format_out[format_out_index - 1] = '\0';
#ifdef MP_16BIT
                     printed_characters += s_mp_print_limb((mp_digit)va_arg(args, unsigned int), format_out, base, print_prefix, stream);
#else
                     printed_characters += s_mp_print_limb(va_arg(args, mp_digit), format_out, base, print_prefix, stream);
#endif
                     break;
                  case 'N':
                     format_out[format_out_index - 2] = *format;
                     format_out[format_out_index - 1] = '\0';
                     printed_characters += s_mp_print_limb_array(va_arg(args, mp_int *), format_out, base, print_prefix, stream);
                     break;

                  }
               } else {
                  printed_characters += fprintf(stream, format_out, va_arg(args, unsigned int));
               }
               break;
            case '@':
               if (modifier == 'Z') {
                  format_out[format_out_index - 2] = 's';
                  format_out[format_out_index - 1] = '\0';
                  printed_characters += s_mp_print_bigint(va_arg(args, mp_int *), format_out, 64, false, stream);
               } else {
                  /* Just assume it is an actual @, to be printed verbatim */
                  fputc(*format, stream);
                  printed_characters++;
               }
               break;
            case 'p':
               printed_characters += fprintf(stream, format_out, va_arg(args, void *));
               break;
            case 'n':
               *va_arg(args, int *) = printed_characters;
               break;
            }
         }
      } else {
         fputc(*format, stream);
         printed_characters++;
      }
      LTM_NEXT(format);
   }
   return printed_characters;
LTM_ERR:
   return -1;
}
#endif /* MP_NO_FILE */
#endif
