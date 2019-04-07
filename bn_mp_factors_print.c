#include "tommath_private.h"
#ifdef BN_MP_FACTORS_PRINT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Print the elememts of a factor-array (an array of mp_int's) */
int mp_factors_print(const mp_factors *f, int base, char delimiter, FILE *stream)
{
   int i, e = MP_OKAY;
   int d;
   if (delimiter != 0) {
      d = (int)delimiter;
   } else {
      d = ',';
   }
   if (f->factors != NULL) {
      for (i = 0; i < f->length; i++) {
         if ((e = mp_fwrite(&(f->factors[i]),base, stream)) != MP_OKAY) {
            return e;
         }
         if (i < ((f->length)-1)) {
            if (fputc(d, stream) == EOF) {
               return MP_VAL;
            }
         }
      }
   }
   return e;
}

#endif

