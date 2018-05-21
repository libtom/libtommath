#include "tommath_private.h"
#ifdef BN_MP_PRIME_FROBENIUS_UNDERWOOD_C

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

#ifdef MP_8BIT
/*
 * floor of positive solution of
 * (2^16)-1 = (a+4)*(2*a+5)
 * TODO: that is too small, would have to use a bigint for a instead
 */
#define LTM_FROBENIUS_UNDERWOOD_A 177
/*
 * Commented out to allow Travis's tests to run
 * Don't forget to switch it back on in production or we'll find it at TDWTF.com!
 */
 /* #warning "Frobenius test not fully usable with MP_8BIT!" */
#else
/*
 * floor of positive solution of
 * (2^31)-1 = (a+4)*(2*a+5)
 * TODO: that might be too small
 */
#define LTM_FROBENIUS_UNDERWOOD_A 32764
#endif
int mp_prime_frobenius_underwood(const mp_int *N, int *result)
{
   mp_int T1z,T2z,Np1z,sz,tz;

   int a, ap2, length, i, j, isset;
   int e = MP_OKAY;

   *result = MP_NO;

   if ((e = mp_init_multi(&T1z,&T2z,&Np1z,&sz,&tz, NULL)) != MP_OKAY) {
      goto LBL_FU_ERR;
   }

   for (a = 0; a < LTM_FROBENIUS_UNDERWOOD_A; a++) {
      /* TODO: That's ugly! No, really, it is! */
      if (a==2||a==4||a==7||a==8||a==10||a==14||a==18||a==23||a==26||a==28) {
         continue;
      }
      /* (32764^2 - 4) < 2^31, no bigint for >MP_8BIT needed) */
      if ((e = mp_set_long(&T1z,(unsigned long)a)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }

      if ((e = mp_sqr(&T1z,&T1z)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }

      if ((e = mp_sub_d(&T1z,4,&T1z)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }

      if ((e = mp_kronecker(&T1z, N, &j)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }

      if (j == -1) {
         break;
      }

      if (j == 0) {
         /* composite */
         goto LBL_FU_ERR;
      }
   }
   if (a >= LTM_FROBENIUS_UNDERWOOD_A) {
      e = MP_VAL;
      goto LBL_FU_ERR;
   }
   /* Composite if N and (a+4)*(2*a+5) are not coprime */
   if ((e = mp_set_long(&T1z, (unsigned long)((a+4)*(2*a+5)))) != MP_OKAY) {
      goto LBL_FU_ERR;
   }

   if ((e = mp_gcd(N,&T1z,&T1z)) != MP_OKAY) {
      goto LBL_FU_ERR;
   }

   if (!(T1z.used == 1 && T1z.dp[0] == 1u)) {
      goto LBL_FU_ERR;
   }

   ap2 = a + 2;
   if ((e = mp_add_d(N,1u,&Np1z)) != MP_OKAY) {
      goto LBL_FU_ERR;
   }

   mp_set(&sz,1u);
   mp_set(&tz,2u);
   length = mp_count_bits(&Np1z);

   for (i = length - 2; i >= 0; i--) {
      /*
       * temp = (sz*(a*sz+2*tz))%N;
       * tz   = ((tz-sz)*(tz+sz))%N;
       * sz   = temp;
       */
      if ((e = mp_mul_2(&tz,&T2z)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }
      /* a = 0 at about 50% of the cases (non-square and odd input) */
      if (a != 0) {
         if ((e = mp_mul_d(&sz,(mp_digit)a,&T1z)) != MP_OKAY) {
            goto LBL_FU_ERR;
         }
         if ((e = mp_add(&T1z,&T2z,&T2z)) != MP_OKAY) {
            goto LBL_FU_ERR;
         }
      }
      if ((e = mp_mul(&T2z, &sz, &T1z)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }
      if ((e = mp_sub(&tz, &sz, &T2z)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }
      if ((e = mp_add(&sz, &tz, &sz)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }
      if ((e = mp_mul(&sz, &T2z, &tz)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }
      if ((e = mp_mod(&tz, N, &tz)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }
      if ((e = mp_mod(&T1z, N, &sz)) != MP_OKAY) {
         goto LBL_FU_ERR;
      }
      if ((isset = mp_get_bit(&Np1z,i)) == MP_VAL) {
         e = isset;
         goto LBL_FU_ERR;
      }
      if (isset == MP_YES) {
         /*
          *  temp = (a+2) * sz + tz
          *  tz   = 2 * tz - sz
          *  sz   = temp
          */
         if (a == 0) {
            if ((e = mp_mul_2(&sz,&T1z)) != MP_OKAY) {
               goto LBL_FU_ERR;
            }
         } else {
            if ((e = mp_mul_d(&sz, (mp_digit) ap2, &T1z)) != MP_OKAY) {
               goto LBL_FU_ERR;
            }
         }
         if ((e = mp_add(&T1z, &tz, &T1z)) != MP_OKAY) {
            goto LBL_FU_ERR;
         }
         if ((e = mp_mul_2(&tz, &T2z)) != MP_OKAY) {
            goto LBL_FU_ERR;
         }
         if ((e = mp_sub(&T2z, &sz, &tz)) != MP_OKAY) {
            goto LBL_FU_ERR;
         }
         mp_exch(&sz,&T1z);
      }
   }

   if ((e = mp_set_long(&T1z, (unsigned long)(2 * a + 5))) != MP_OKAY) {
      goto LBL_FU_ERR;
   }
   if ((e = mp_mod(&T1z,N,&T1z)) != MP_OKAY) {
      goto LBL_FU_ERR;
   }
   if (mp_iszero(&sz) && (mp_cmp(&tz, &T1z) == MP_EQ)) {
      *result = MP_YES;
      goto LBL_FU_ERR;
   }

LBL_FU_ERR:
   mp_clear_multi(&T1z,&T2z,&Np1z,&sz,&tz, NULL);
   return e;
}

#endif
