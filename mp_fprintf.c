#include "tommath_private.h"
#ifdef MP_FPRINTF_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef MP_NO_FILE
#include <stdarg.h>
int mp_fprintf(FILE *stream, const char *format, ...)
{
   int written = 0;

   va_list args;
   va_start(args, format);
   written = s_mp_fprintf(stream, format, args);
   va_end(args);

   return written;

}
#endif
#endif
