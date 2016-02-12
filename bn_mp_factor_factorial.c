#include <tommath.h>
#ifdef BN_MP_FACTOR_FACTORIAL_C
#ifndef LN_113
#define LN_113 1.25505871293247979696870747618124469168920275806274
#endif
#include <math.h>
int mp_factor_factorial(unsigned long n, unsigned long start,
                        long **prime_list,unsigned long *length)
{

   unsigned long pix=0,prime,K,test_before;
   mp_bitset_t *bst;

   /* To be able to handle negative values in subtraction and to be sure that
      n is positive*/
   if (n > LONG_MAX) {
      return MP_VAL;
   }

   bst = malloc(sizeof(mp_bitset_t));
   if (bst == NULL) {
      return MP_MEM;
   }
   mp_bitset_alloc(bst, n+1);
   mp_eratosthenes(bst);
   pix = (unsigned long)(LN_113 *  n/log(n)) + 2 ;

   *prime_list = malloc(sizeof(long) * (pix) * 2);
   if (*prime_list == NULL) {
      return MP_MEM;
   }
   if (start <= 2) {
      prime = 2;
   } else {
      prime = start;
   }
   K=0;
   do {
      (*prime_list)[K]   = (long)prime;
      test_before = mp_prime_divisors(n,prime);
      if (test_before > LONG_MAX) {
         free(bst);
         free(*prime_list);
         return MP_VAL;
      }
      (*prime_list)[K+1] = (long)test_before;
      prime              = mp_bitset_nextset(bst,prime+1);
      K+=2;
   } while (prime <= n);
   *prime_list = realloc(*prime_list,sizeof(long) * K);
   if (*prime_list == NULL) {
      return MP_MEM;
   }
   free(bst);
   *length = K;
   return MP_OKAY;
}

#endif
