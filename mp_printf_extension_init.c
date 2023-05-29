#include "tommath_private.h"
#ifdef MP_PRINTF_EXTENSION_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#if (!(defined(_MSC_VER)) &&  !(defined(MP_NO_FILE)) && (defined(GLIBC_VERSION)) && (GLIBC_VERSION > 2009) )
#include <printf.h>

static int modZ = -1;
static int modM = -1;
static int modN = -1;

/* Size of mp_digit at base 2 rounded up */
#define MP_LIMB_BUFSIZ 64
static int s_mp_print_mp_int(FILE *stream,  const struct printf_info *info,  const void *const *args)
{
   mp_err err = MP_OKAY;

   const mp_int *a = NULL;
   mp_digit d = 0;

   char *buf, *start_buf;
   char limb_buffer[MP_LIMB_BUFSIZ] = {0};

   int base = 10;
   size_t size = 0, written = 0;
   int len, minwidth, res = 0;
   size_t extra_len = 0u;

   int idx = 0, fill_zeros = 0, i=0;

   char letter = '\0';

   bool is_mp_digit = false;

   if (info->user & modM) {
      is_mp_digit = true;
   }

   /* Fiddle our number out of the argument list */
   if (is_mp_digit) {
      d = *((mp_digit const *)(args[0]));
   } else {
      a = *((const mp_int * const *)(args[0]));
   }

   switch (info->spec) {
   case L'd':
   case L'i':
   case L'u':
      base = 10;
      break;
   case L'x':
      base = 16;
      letter = 'x';
      break;
   case L'b':
      base = 2;
      letter = 'b';
      break;
   case L'o':
      base = 8;
      letter = 'o';
      break;
   case L'@':
      base = 64;
      letter = '@';
      break;
   }

   /* Print the raw array a->dp, with fixed formatting */
   if (info->user & modN) {
      for (idx = 0; idx < a->used; idx++) {
         len = s_mp_print_mp_digit(a->dp[idx], (mp_digit)base, true, limb_buffer);
         res += len;
         for (i = 0; i < len; i++) {
            MP_FPUTC(limb_buffer[i],stream);
         }
         /* TODO: offer sth to change it? */
         MP_FPUTC(',',stream);
         res++;
      }
      return res;
   }

   if (!is_mp_digit) {
      /* Get some estimate of the size of "a" in the given base */
      if ((err = mp_radix_size_overestimate(a, base, &size)) != MP_OKAY) {
         return -1;
      }
   } else {
      /* 64 bit at most. With binary repres. we need
         sign + prefix + number_in_base_2
           1  +   2    +      64           = 67
       */
      size = MP_LIMB_BUFSIZ + 4;
   }

   /* The minus sign comes before the "0x", we have to take precaution */
   if ((base != 10) && (info->alt == 1u)) {
      extra_len += 2;
   }

   if (s_mp_isneg(a) || (info->space) || (info->showsign)) {
      extra_len++;
   }

   if (info->prec > 0) {
      if (info->prec > (int)size) {
         /* exact number is in "written" later */
         size = (size_t)info->prec;
      }
   }

   buf = MP_MALLOC(size + extra_len);
   if (buf == NULL) {
      return -1;
   }

   /* Keep a feet on the beginning of the rope. */
   start_buf = buf;
   /* Make some space for the prefix and the sign */
   buf += extra_len;

   if (!is_mp_digit) {
      /* We overwrite the minus-sign */
      if (mp_isneg(a)) {
         buf--;
      }
      if ((err = mp_to_radix(a, buf, size, &written, base)) != MP_OKAY) {
         MP_FREE(start_buf, size + extra_len);
         return -1;
      }
   } else {
      written = (size_t)s_mp_print_mp_digit(d, (mp_digit)base, false, limb_buffer);
   }

   /* RTL is not supported */
   if (s_mp_isneg(a)) {
      start_buf[idx++] = '-';
   } else  {
      if (info->space) {
         start_buf[idx++] = ' ';
      } else if (info->showsign) {
         start_buf[idx++] = '+';
      }
   }

   /* TODO: Octal is "0o" now as proposed in C2x and Posix */
   if ((base != 10) && (info->alt == 1u)) {
      start_buf[idx++] = '0';
      start_buf[idx++] = letter;
   }

   /* Length of the number with the prefix but without the leading zeros (prec) */
   len = idx + (int)written - 1;
   /* We overwrite the minus */
   if (s_mp_isneg(a)) {
      len--;
   }

   if (info->prec > len) {
      minwidth = info->width - info->prec;
   } else {
      minwidth = info->width - len;
   }

   /* Padding for right alignment */
   if (!(info->left) && (minwidth > 0)) {
      while (minwidth--) {
         /* TODO: The type of info->pad is wchar_t and writable. Offer?
                  This branching can go if not.
          */
         if (info->pad != 0) {
            MP_FPUTC((int)(info->pad),stream);
            res++;
         } else {
            MP_FPUTC(' ',stream);
            res++;
         }
      }
   }

   /* Print preliminaries */
   for (i = 0; i < idx; i++) {
      MP_FPUTC(start_buf[i],stream);
      res++;
   }

   if (info->prec > 0) {
      /* Prefix length, if any, is in idx, including minus sign */
      fill_zeros = info->prec - idx - (int)written + 1;
      /* Do nothing if fill_zeros <= 0; no truncating */
      if (fill_zeros > 0) {
         while (fill_zeros--) {
            MP_FPUTC('0',stream);
            res++;
         }
      }
   }

   /* Print the actual number */
   if (!is_mp_digit) {
      while (start_buf[idx] != '\0') {
         MP_FPUTC(start_buf[idx++],stream);
         res++;
      }
   } else {
      for (i = 0; i < (int)written; i++) {
         MP_FPUTC(limb_buffer[i],stream);
         res++;
      }
   }

   /* Padding for left alignment */
   if ((info->left) && (minwidth > 0)) {
      while (minwidth--) {
         if (info->pad != 0) {
            MP_FPUTC((int)(info->pad),stream);
            res++;
         } else {
            MP_FPUTC(' ',stream);
            res++;
         }
      }
   }

   MP_FREE(start_buf, size + extra_len);
   return res;
}
static int s_mp_print_mp_int_arginfo(const struct printf_info *info, size_t n,
                                     int *argtypes, int *size)
{
   /*
      Using the information in that struct raises some massive memory
      problems as listed by Valgrind. Might be a problem with Valgrind
      because the struct is declared in the parameter list.
    */
   (void)(info);

   if ((n > 0) && (info->user & modZ)) {
      argtypes[0] = PA_POINTER;
      size[0] = sizeof(mp_int *);
   }
   if ((n > 0) && (info->user & modM)) {
#if (MP_DIGIT_BIT > 32)
      argtypes[0] = PA_INT | PA_FLAG_LONG_LONG;
#else
      argtypes[0] = PA_INT | PA_FLAG_LONG;
#endif
      size[0] = sizeof(mp_digit);
   }

   if ((n > 0) && (info->user & modN)) {
#if (MP_DIGIT_BIT > 32)
      argtypes[0] = PA_INT | PA_FLAG_LONG_LONG;
#else
      argtypes[0] = PA_INT | PA_FLAG_LONG;
#endif
      size[0] = sizeof(mp_digit);
   }
   return 1;
}
/* Register printf extensions */
mp_err mp_printf_extension_init(void)
{
   mp_err err = MP_OKAY;

   /* Big integer (mp_int) */
   modZ = register_printf_modifier(L"Z");
   if (modZ < 0) {
      return MP_VAL;
   }

   /* Limb (mp_digit) */
   modM = register_printf_modifier(L"M");
   if (modM < 0) {
      return MP_VAL;
   }
   /* raw mp_int array: a->dp */
   modN = register_printf_modifier(L"N");
   if (modN < 0) {
      return MP_VAL;
   }

   /*
       Caveat: these are bound to our modifiers now, using them normaly
       like in print("%ld", (long)123) may not work anymore.
    */
   register_printf_specifier('d', s_mp_print_mp_int, s_mp_print_mp_int_arginfo);
   register_printf_specifier('x', s_mp_print_mp_int, s_mp_print_mp_int_arginfo);
   register_printf_specifier('o', s_mp_print_mp_int, s_mp_print_mp_int_arginfo);
   register_printf_specifier('b', s_mp_print_mp_int, s_mp_print_mp_int_arginfo);
   register_printf_specifier('@', s_mp_print_mp_int, s_mp_print_mp_int_arginfo);

   return err;
}
#else
/* Dummy functions if the functions in printf.h are not supported */
mp_err mp_printf_extension_init(void)
{
   return MP_VAL;
}
#endif


#endif
