#include <tommath.h>
#ifdef BN_MP_EULER_C


/*
@ARTICLE{2011arXiv1108.0286B,
   author = {{Brent}, R.~P. and {Harvey}, D.},
    title = "{Fast computation of Bernoulli, Tangent and Secant numbers}",
  journal = {ArXiv e-prints},
     year = 2011,
    month = aug,
   howpublished = {\url{http://adsabs.harvard.edu/abs/2011arXiv1108.0286B}},
   note = {\url{http://adsabs.harvard.edu/abs/2011arXiv1108.0286B}}
}
*/


static  unsigned long euler_array_size = 0;
static  mp_int *euler_array;

static int bheuler(unsigned long limit)
{
   mp_int tmp;
   unsigned long j,k,N;
   int e;

   N = limit+1;

   if (N < euler_array_size) {
      //printf("Cache hit: %lu\n",N);
      return MP_OKAY;
   }

   /* For sake of simplicity */
   euler_array = malloc(sizeof(mp_int)*N+2);
   if (euler_array == NULL) {
      return MP_MEM;
   }
   for (k=0; k<=N; k++) {
      if ((e = mp_init(&euler_array[k])) != MP_OKAY) {
         return e;
      }
   }
   if ((e = mp_set_int(&euler_array[0],1)) != MP_OKAY) {
      return e;
   }
   for (k = 1; k<N; k++) {
      /* euler_array[k] = (k)*euler_array[k-1] */
      if ((e = mp_mul_d(&euler_array[k-1],(k),&euler_array[k])) != MP_OKAY) {
         return e;
      }
   }

   if ((e =  mp_init(&tmp)) != MP_OKAY) {
      return e;
   }

   for (k = 1; k<N; k++) {
      for (j = k+1; j< N; j++) {
         /* euler_array[j] =  (j-k)*euler_array[j-1]  +   (j-k+1)*euler_array[j] */
         /* tmp  =  (j-k)*euler_array[j-1]  */
         if ((e = mp_mul_d(&euler_array[j-1],(j-k),&tmp)) != MP_OKAY) {
            return e;
         }
         /* euler_array[j] =   (j-k+1)*euler_array[j] */
         if ((e = mp_mul_d(&euler_array[j],(j-k+1),&euler_array[j])) != MP_OKAY) {
            return e;
         }
         /* euler_array[j] =   euler_array[j]  + tmp*/
         if ((e = mp_add(&euler_array[j],&tmp,&euler_array[j])) != MP_OKAY) {
            return e;
         }
      }
   }
   for (k = 0; k<N; k++) {
      /* Odd Euler numbers are negative */
      if (k&0x1) {
         (euler_array[k]).sign = MP_NEG;
      }
   }
   euler_array_size = N;
   /* It is deemed good style to clean up after work */
   mp_clear(&tmp);
   return MP_OKAY;
}


int mp_euler(unsigned long n, mp_int *c)
{
   int e,k;

   /* all odd Euler numbers are zero */
   if ((n&0x1) && n != 1) {
      mp_set_int(c,0);
      return MP_OKAY;
   }
   if ((e =  bheuler(n/2)) != MP_OKAY) {
      return e;
   }
   k = n/2;
   if ((e =  mp_copy(&euler_array[k],c)) != MP_OKAY) {
      return e;
   }
   return MP_OKAY;
}
void mp_euler_free(void)
{
   unsigned long i=0;
   for (; i<euler_array_size; i++) {
      mp_clear(&(euler_array[i]));
   }
   free(euler_array);
}

#endif
