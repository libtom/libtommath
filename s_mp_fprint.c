#include "tommath_private.h"
#ifdef S_MP_FPRINT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef MP_NO_FILE
#ifdef MP_WITH_MP_FPRINTF
int s_mp_fprint(const mp_int *a, size_t maxlen, unsigned int flags, int radix, FILE *stream)
{
   char *buffer = NULL;
   int written = 0, printed = -1;

   written = s_mp_sprint(a, maxlen, flags, radix, &buffer);
   if (written < 0) {
      goto LTM_ERR;
   }
   if (buffer != NULL) {
      printed = fprintf(stream,"%s", buffer);
   }
   if (printed < 0) {
      written = printed;
      goto LTM_ERR;
   }
   printed++;
   if (written != printed) {
      /* fprintf(stderr, "written (%d) != printed (%d)\n", written, printed); */
      goto LTM_ERR;
   }

LTM_ERR:
   MP_FREE_BUF(buffer, (size_t)written);
   return written;
}
#endif
#endif
#endif
