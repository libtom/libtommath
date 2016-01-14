#include <tommath.h>
#ifdef BN_MP_COMPUTE_FACTORED_FACTORIAL_C

static long local_highbit(unsigned  long n)
{
   long r=0;
   while (n >>= 1) {
      r++;
   }
   return r;
}

unsigned long BN_MP_FACTORED_FACTORIAL_CUTOFF = 0x10000000;
/* Input is to be sorted as: prime,exponent,prime,exponent,... */
int mp_compute_factored_factorial(unsigned long *f, unsigned long f_length,
                                  mp_int *c,   unsigned long stop)
{

   unsigned long length,start,i;
   unsigned long shift = 0;
   long bit,k=0,hb;
   mp_int temp;
   int e;

   if (stop == 0) {
      length = f_length;
   } else {
      length = stop;
   }
   if (f[0] == 2 && f[1] > 0) {
      shift = f[1];
      if (f_length == 2) {
         if ((e = mp_set_int(c,1LU<<f[1])) != MP_OKAY) {
            return e;
         }
         return MP_OKAY;
      }
   }

   start = 0;
   if (shift) {
      start+=2;
      k=2;
   }
   bit = 0;
   for (; k<(long)f_length; k+=2) {
      hb=local_highbit(f[k+1]);
      if (bit < hb)bit=hb;
   }
   /* just for safety reasons, y'know */
   if (bit >(long) DIGIT_BIT) {
      return MP_VAL;
   }
   if ((e = mp_set_int(c,1)) != MP_OKAY) {
      return e;
   }
   if (f_length > BN_MP_FACTORED_FACTORIAL_CUTOFF) {
      if ((e = mp_init(&temp)) != MP_OKAY) {
         return e;
      }
      for (; bit>=0; bit--) {
         if ((e =  mp_sqr(c, c)) != MP_OKAY) {
            return e;
         }
         if ((e =  mp_set_int(&temp,1)) != MP_OKAY) {
            return e;
         }
         for (i=start; i<f_length; i+=2) {
            if ((f[i+1] & (1LU << (unsigned long)bit)) != 0) {
               if ((e =  mp_mul_d(&temp,(mp_digit)f[i], &temp)) != MP_OKAY) {
                  return e;
               }
            }
         }
         if ((e =  mp_mul(&temp,c,c)) != MP_OKAY) {
            return e;
         }
      }
   } else {
      for (; bit>=0; bit--) {
         if ((e = mp_sqr(c, c)) != MP_OKAY) {
            return e;
         }
         for (i=start; i<f_length; i+=2) {
            if ((f[i+1] & (1LU << (unsigned long)bit)) != 0) {
               if ((e = mp_mul_d(c,(mp_digit)f[i], c)) != MP_OKAY) {
                  return e;
               }
            }
         }
      }
   }
   if (shift) {
      /* DIGIT_BIT might be 60 which makes this extra check necessary */
      if (shift < INT_MAX) {
         /* The choice of types is a bit questionable. Sometimes. */
         if ((e = mp_mul_2d(c, (int) shift, c)) != MP_OKAY) {
            return e;
         }
      } else {
         int multiplicator=0;
         while (shift > INT_MAX) {
            shift >>= 1;
            multiplicator++;
         }
         if ((e = mp_mul_2d(c, (int) shift, c)) != MP_OKAY) {
            return e;
         }
         if ((e = mp_mul_2d(c, 1<<multiplicator, c)) != MP_OKAY) {
            return e;
         }
      }
   }
   return MP_OKAY;
}
#endif
