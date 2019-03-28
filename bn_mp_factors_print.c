#include "tommath_private.h"
#ifdef BN_MP_FACTORS_PRINT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * SPDX-License-Identifier: Unlicense
 */



/* Print the elememts of a factor-array (an array of mp_int's) */
#ifdef LTM_USE_EXTRA_FUNCTIONS
int mp_factors_print(mp_factors *f, int base, char delimiter, FILE *stream)
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
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
