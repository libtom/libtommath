#include "tommath_private.h"
#ifdef BN_MP_MUL_SI_C

/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 */

// multiply bigint a with int d and put the result in c
// Like mp_mul_d() but with a signed long as the small input
int mp_mul_si(const mp_int *a, long d, mp_int *c)
{
   mp_int t;
   int err, neg = 0;

   if ((err = mp_init(&t)) != MP_OKAY) {
      return err;
   }
   if (d < 0) {
      neg = 1;
      d = -d;
   }

   // mp_digit might be smaller than a long, which excludes
   // the use of mp_mul_d() here.
   if ((err = mp_set_int(&t, (unsigned long) d)) != MP_OKAY) {
      goto LBL_MPMULSI_ERR;
   }
   if ((err = mp_mul(a, &t, c)) != MP_OKAY) {
      goto LBL_MPMULSI_ERR;
   }
   if (neg ==  1) {
      c->sign = (a->sign == MP_NEG) ? MP_ZPOS: MP_NEG;
   }
LBL_MPMULSI_ERR:
   mp_clear(&t);
   return err;
}



#endif
