#include <tommath.h>
#ifdef BN_MP_FALLING_FACTORIAL_C
#ifndef LN_113
#   define LN_113 1.25505871293247979696870747618124469168920275806274
#endif
#include <math.h>

int mp_falling_factorial(unsigned long n, unsigned long k, mp_int *c)
{
   unsigned long *prime_list;
   unsigned long pix = 0, prime, K, diff;
   mp_bitset_t *bst;

   int e;

   if (n < k) {
      mp_set(c, 0);
      return MP_OKAY;
   }
   if (k == 0) {
      mp_set(c, 1);
      return MP_OKAY;
   }
   if (k == 1) {
      if ((e = mp_set_int(c, n)) != MP_OKAY) {
         return e;
      }
      return MP_OKAY;
   }
   if (k == n) {
      if ((e = mp_factorial(n, c)) != MP_OKAY) {
         return e;
      }
      return MP_OKAY;
   }

   bst = malloc(sizeof(mp_bitset_t));
   if (bst == NULL) {
      return MP_MEM;
   }
   mp_bitset_alloc(bst, n + 1);
   mp_eratosthenes(bst);
   /* One could also count the number of primes in the already filled sieve */
   pix = (unsigned long)(LN_113 * n / log(n)) + 2;

   prime_list = malloc(sizeof(unsigned long) * (pix) * 2);
   if (prime_list == NULL) {
      return MP_MEM;
   }
   prime = 2;
   K = 0;
   do {
      diff = mp_prime_divisors(n, prime) - mp_prime_divisors(n - k, prime);
      if (diff != 0) {
         prime_list[K] = prime;
         prime_list[K + 1] = diff;
         K += 2;
      }
      prime = mp_bitset_nextset(bst, prime + 1);
   } while (prime <= n - k);
   do {
      prime_list[K] = prime;
      prime_list[K + 1] = mp_prime_divisors(n, prime);
      prime = mp_bitset_nextset(bst, prime + 1);
      K += 2;
   } while (prime <= n);
   prime_list = realloc(prime_list, sizeof(unsigned long) * K);
   if (prime_list == NULL) {
      return MP_MEM;
   }
   if ((e = mp_compute_factored_factorial(prime_list, K - 1, c, 0)) != MP_OKAY) {
      return e;
   }
   free(bst);
   free(prime_list);

   return MP_OKAY;
}
#endif
