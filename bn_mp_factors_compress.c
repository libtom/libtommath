#include "tommath_private.h"
#ifdef BN_MP_FACTORS_COMPRESS_C
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


/* Change (sorted) factors array into format prime,prime-count,...,prime,prime-count */
int mp_factors_compress(mp_factors *f, int sorted, mp_factors *g)
{
   int i, e = MP_OKAY;
   mp_int count;
   mp_factors in, out, t;

   if ((e = mp_init(&count)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_factors_init(&out)) != MP_OKAY) {
      goto LTM_ERR;
   }

   if ((e = mp_factors_init(&in)) != MP_OKAY) {
      goto LTM_ERR;
   }
   for (i = 0; i < f->length; i++) {
      if ((e = mp_factors_add(&(f->factors[i]), &in)) != MP_OKAY) {
         goto LTM_ERR;
      }
   }
   if (sorted == MP_NO) {
      mp_factors_sort(&in);
   }

   for (i = 1; i < in.length; i++) {
      if ((e = mp_incr(&count)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if (mp_cmp(&(in.factors[i-1]), &(in.factors[i])) != MP_EQ) {
         if ((e = mp_factors_add(&(in.factors[i-1]), &out)) != MP_OKAY) {
            goto LTM_ERR;
         }
         if ((e = mp_factors_add(&count, &out)) != MP_OKAY) {
            goto LTM_ERR;
         }
         mp_zero(&count);
      }
   }
   /* and the last one, too */
   if ((e = mp_incr(&count)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_factors_add(&(in.factors[i-1]), &out)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_factors_add(&count, &out)) != MP_OKAY) {
      goto LTM_ERR;
   }

   t = *g;
   *g = out;
   out = t;

LTM_ERR:
   mp_factors_clear(&in);
   mp_clear(&count);
   mp_factors_clear(&out);
   return e;
}

#endif
/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
