#include "tommath_private.h"
#ifdef MP_FPRINTF_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef MP_NO_FILE
#ifdef MP_WITH_MP_FPRINTF
/* At most 2*int (might be 64 bit) -> 2*20 plus
   some change, so 128 should more than enough */
#define MP_FPRINTF_BUF_LEN 128
int mp_fprintf(FILE *stream, const char *s, ...)
{
   mp_err err = MP_OKAY;
   printf_token token;
   va_list args;
   char buf[MP_FPRINTF_BUF_LEN];
   ptrdiff_t len;
   int i, count = 0;
   int *enn = NULL;
   size_t maxlen;
   int printed = 0;
   bool valid_token = false;
   mp_int t;

   /* For the people who switch off all compiler warnings */
   if (s == NULL) {
      return -1;
   }

   if ((err = mp_init(&t)) != MP_OKAY) {
      return -1;
   }

   va_start(args, s);

   while ((*s) != '\0') {
      if (*s == '%') {
         valid_token = s_mp_parse_printf_token(s, &token);
         if (!valid_token) {
            printed = -1;
            goto LTM_ERR;
         }
         len = token.end_ptr - token.start_ptr;
         /* TODO: Can that even happen if parse_printf_token returns true? */
         if (len == 0) {
            printed = -1;
            goto LTM_ERR;
         }
         /* Aren't we already there? */
         s = token.start_ptr;
         /* Put the whole thing in a buffer and let fprintf do the heavy work */
         for (i = 0; i < (int) len; i++) {
            buf[i] = *s;
            s++;
         }
         buf[i] = '\0';

         switch (token.specifier) {
         case 'd':
         case 'i':
            switch (token.length) {
            case LEN_h:
               /* Integer promotion: never smaller than an int/double (7.16.1.1 "The va_arg macro" par. 2) */
               count = fprintf(stream, buf, va_arg(args, int));
               if (count >= 0) {
                  printed += count;
               } else {
                  /* fprintf() will set errno/ferror() accordingly */
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_hh:
               count = fprintf(stream, buf, va_arg(args, int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_l:
               count = fprintf(stream, buf, va_arg(args, long int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_ll:
               count = fprintf(stream, buf, va_arg(args, long long int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_j:
               count = fprintf(stream, buf, va_arg(args, intmax_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_z: /*TODO: error?*/
               count = fprintf(stream, buf, va_arg(args, size_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_t:
               count = fprintf(stream, buf, va_arg(args, ptrdiff_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_Z:
               maxlen = 0;
               if (token.width > 0) {
                  maxlen = (size_t)token.width;
               }
               if (token.width == MP_PRINTF_DYNAMIC) {
                  mp_set(&t, 0u);
                  if ((err = mp_copy(va_arg(args, mp_int *),&t)) != MP_OKAY)        goto LTM_ERR;
                  count = s_mp_fprint(&t, (size_t)va_arg(args, int), token.flags, 10, stream);
               } else {
                  count = s_mp_fprint(va_arg(args, mp_int *), maxlen, token.flags, 10, stream);
               }
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            default:
               count = fprintf(stream, buf, va_arg(args, int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            }
            break;
         case 'u':
            switch (token.length) {
            case LEN_h:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_hh:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_l:
               count = fprintf(stream, buf, va_arg(args, long unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_ll:
               count = fprintf(stream, buf, va_arg(args, long long unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_j:
               count = fprintf(stream, buf, va_arg(args, uintmax_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_z:
               count = fprintf(stream, buf, va_arg(args, size_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_t:
               count = fprintf(stream, buf, va_arg(args, ptrdiff_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_Z:
               /* This is unsigned, show absolute value?  */
               maxlen = 0;
               if (token.width > 0) {
                  maxlen = (size_t)token.width;
               }
               if (token.width == MP_PRINTF_DYNAMIC) {
                  mp_set(&t, 0u);
                  if ((err = mp_copy(va_arg(args, mp_int *), &t)) != MP_OKAY)        goto LTM_ERR;
                  count = s_mp_fprint(&t, (size_t)va_arg(args, int), token.flags, 10, stream);
               } else {
                  count = s_mp_fprint(va_arg(args, mp_int *), maxlen, token.flags, 10, stream);
               }
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            default:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               break;
            }
            break;
         case 'o':
            switch (token.length) {
            case LEN_h:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_hh:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_l:
               count = fprintf(stream, buf, va_arg(args, long unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_ll:
               count = fprintf(stream, buf, va_arg(args, long long unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_j:
               count = fprintf(stream, buf, va_arg(args, uintmax_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_z:
               count = fprintf(stream, buf, va_arg(args, size_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_t:
               count = fprintf(stream, buf, va_arg(args, ptrdiff_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_Z:
               maxlen = 0;
               if (token.width > 0) {
                  maxlen = (size_t)token.width;
               }
               if (token.width == MP_PRINTF_DYNAMIC) {
                  mp_set(&t, 0u);
                  if ((err = mp_copy(va_arg(args, mp_int *),&t)) != MP_OKAY)        goto LTM_ERR;
                  count = s_mp_fprint(&t, (size_t)va_arg(args, int), token.flags, 8, stream);
               } else {
                  count = s_mp_fprint(va_arg(args, mp_int *), maxlen, token.flags, 8, stream);
               }
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            default:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            }
            break;

         case 'x':
         case 'X':
            switch (token.length) {
            case LEN_h:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_hh:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_l:
               count = fprintf(stream, buf, va_arg(args, long unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_ll:
               count = fprintf(stream, buf, va_arg(args, long long unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_j:
               count = fprintf(stream, buf, va_arg(args, uintmax_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_z:
               count = fprintf(stream, buf, va_arg(args, size_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_t:
               count = fprintf(stream, buf, va_arg(args, ptrdiff_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_Z:
               /* TODO: add upper-/lowercase */
               maxlen = 0;
               if (token.width > 0) {
                  maxlen = (size_t)token.width;
               }
               if (token.width == MP_PRINTF_DYNAMIC) {
                  mp_set(&t, 0u);
                  if ((err = mp_copy(va_arg(args, mp_int *), &t)) != MP_OKAY)        goto LTM_ERR;
                  count = s_mp_fprint(&t, (size_t)va_arg(args, int), token.flags, 16, stream);
               } else {
                  count = s_mp_fprint(va_arg(args, mp_int *), maxlen, token.flags, 16, stream);
               }
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            default:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            }
            break;
         /* Has been added lately (C23) to the standard */
         case 'b':
         case 'B':
            switch (token.length) {
#if ( __STDC_VERSION__ >= 202000 )
            case LEN_h:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_hh:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_l:
               count = fprintf(stream, buf, va_arg(args, long unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_ll:
               count = fprintf(stream, buf, va_arg(args, long long unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_j:
               count = fprintf(stream, buf, va_arg(args, uintmax_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_z:
               count = fprintf(stream, buf, va_arg(args, size_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            case LEN_t:
               count = fprintf(stream, buf, va_arg(args, ptrdiff_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
#endif
            case LEN_Z:
               maxlen = 0;
               if (token.width > 0) {
                  maxlen = (size_t)token.width;
               }
               if (token.width == MP_PRINTF_DYNAMIC) {
                  mp_set(&t, 0u);
                  if ((err = mp_copy(va_arg(args, mp_int *),&t)) != MP_OKAY)        goto LTM_ERR;
                  count = s_mp_fprint(&t, (size_t)va_arg(args, int), token.flags, 2, stream);
               } else {
                  mp_set(&t, 0u);
                  count = s_mp_fprint(va_arg(args, mp_int *), maxlen, token.flags, 2, stream);
               }
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            default:
               count = fprintf(stream, buf, va_arg(args, unsigned int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            }
            break;
         case 'K':
            switch (token.length) {
            case LEN_Z:
               maxlen = 0;
               if (token.width > 0) {
                  maxlen = (size_t)token.width;
               }
               if (token.width == MP_PRINTF_DYNAMIC) {
                  mp_set(&t, 0u);
                  if ((err = mp_copy(va_arg(args, mp_int *),&t)) != MP_OKAY)        goto LTM_ERR;
                  count = s_mp_fprint(&t, (size_t)va_arg(args, int), token.flags, 64, stream);
               } else {
                  count = s_mp_fprint(va_arg(args, mp_int *), maxlen, token.flags, 64, stream);
               }
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
               break;
            default:
               break;
            }
            break;
         case 'c':
            if (token.length == LEN_l) {
               count = fprintf(stream, buf, va_arg(args, wint_t));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
            } else {
               count = fprintf(stream, buf, va_arg(args, int));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
            }
            break;
         case 's':
            if (token.length == LEN_l) {
               count = fprintf(stream, buf, va_arg(args, const wint_t *));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
            } else  {
               count = fprintf(stream, buf, va_arg(args, const char *));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
            }
            break;
         case 'p':
            count = fprintf(stream, buf, va_arg(args, void *));
            if (count >= 0) {
               printed += count;
            }
            break;
         case 'e':
         case 'E':
         case 'f':
         case 'F':
         case 'g':
         case 'G':
         case 'a':
         case 'A':
            if (token.length == LEN_L) {
               count = fprintf(stream, buf, va_arg(args, long double));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
            } else  {
               count = fprintf(stream, buf, va_arg(args, double));
               if (count >= 0) {
                  printed += count;
               } else {
                  printed = count;
                  goto LTM_ERR;
               }
            }
            break;
         case '%':
            count = fprintf(stream, buf, '%');
            if (count >= 0) {
               printed += count;
            } else {
               printed = count;
               goto LTM_ERR;
            }
            break;
         case 'n':
            /* No output says the standard */
            enn = va_arg(args, int *);
            *enn = printed;
            break;
         default:
            count = fprintf(stream, " unkown specifier: %c\n",token.specifier);
            if (count >= 0) {
               printed += count;
            } else {
               printed = count;
               goto LTM_ERR;
            }
            break;
         }
      }
      count = fprintf(stream, "%c",*s);
      if (count >= 0) {
         printed += count;
      } else {
         printed = count;
         goto LTM_ERR;
      }
      s++;
   }
LTM_ERR:
   if (err != MP_OKAY) {
      printed = -1;
   }
   mp_clear(&t);
   va_end(args);
   return printed;
}

#endif
#endif
#endif
