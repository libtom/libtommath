#include <tommath.h>
#ifdef BN_MP_DOUBLEFACTORIAL_C
#ifndef LN_113 
#define LN_113 1.25505871293247979696870747618124469168920275806274
#endif
#include <math.h>
/* For positve n only. For now */
int mp_doublefactorial(unsigned long n,mp_int *c){
  unsigned long *prime_list;
  unsigned long pix=0,prime,K,diff;
  mp_bitset_t *bst;
  int e;

  /* it is df(2n + 1) = (2*n)!/(n!*2^n) for odd n */
  if(n >= ULONG_MAX/2){
    return MP_VAL;
  }

    switch(n){
      case -1:
      case 0 : mp_set(c,(mp_digit)1);
               return MP_OKAY;
               break;
      case 2 : mp_set(c,(mp_digit)2);
               return MP_OKAY;
               break;
      case 3 : mp_set(c,(mp_digit)3);
               return MP_OKAY;
               break;
      case 4 : mp_set(c,(mp_digit)8);
               return MP_OKAY;
               break;
      case 5 : mp_set(c,(mp_digit)15);
               return MP_OKAY;
               break;
      case 6 : mp_set(c,(mp_digit)48);
               return MP_OKAY;
               break;
      /* smallest DIGIT_BIT is 8 */
      case 7 : mp_set(c,(mp_digit)105);
               return MP_OKAY;
               break;
      default: break;
    }
  
    if(n&0x1){
      n = (n+1)/2;

      bst = malloc(sizeof(mp_bitset_t));
      if (bst == NULL) {
        return MP_MEM;
      }
      mp_bitset_alloc(bst, 2*n+1);
      mp_eratosthenes(bst);

      pix = (unsigned long) (LN_113 *  (2*n)/log(2*n) );

      prime_list = malloc(sizeof(unsigned long) * (pix) * 2);
      if (prime_list == NULL) {
        return MP_MEM;
      }
      prime = 3; K = 0;
      do {
            diff = mp_prime_divisors(2*n,prime) - mp_prime_divisors(n,prime);
            if(diff != 0){
              prime_list[K]   = prime;
              prime_list[K+1] = diff;
              K+=2;
            }
            prime             = mp_bitset_nextset(bst,prime+1);
      } while(prime <= n);
      do {
        prime_list[K] = prime;
        prime_list[K+1] = mp_prime_divisors(2*n,prime);
        K+=2;
        prime             = mp_bitset_nextset(bst,prime+1);
      } while(prime <= 2*n);
      prime_list = realloc(prime_list,sizeof(unsigned long) * K);
      if (prime_list == NULL) {
        return MP_MEM;
      }
      if( (e = mp_compute_factored_factorial(prime_list,K,c,0) ) != MP_OKAY){
        return e;
      }
      free(bst);
      free(prime_list);
      return MP_OKAY;
    }
    else{
      /* Even n */
      n >>= 1;
      mp_factorial(n,c);
      if(n<INT_MAX){
        if( (e = mp_mul_2d (c, (int) n, c) ) != MP_OKAY){
          return e;
        }
      }
      else{
        int multiplicator=0;
        while(n > INT_MAX){
          n >>= 1;
          multiplicator++;
        }
        if( (e = mp_mul_2d (c, (int) n, c) ) != MP_OKAY){
          return e;
        }
        if( (e = mp_mul_2d (c, 1<<multiplicator, c) ) != MP_OKAY){
          return e;
        }
      }
      return MP_OKAY;
    }
    return MP_VAL;
} 

#endif
