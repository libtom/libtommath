#include <tommath.h>
#ifdef BN_MP_BINOMIAL_C
#ifndef LN_113 
#define LN_113 1.25505871293247979696870747618124469168920275806274
#endif
#include <math.h>

#if 0
unsigned long comb_old(unsigned long n, unsigned long k){
	unsigned long i;
        unsigned long long r = 1LLU;
        if(n<k)return 0;
        if(n==k)return 1;
        if(n > 34) return ULONG_MAX;
	if (k > n/2){
           k = n-k;
        }
        for (i=1; i<=k;i++){
          r = (r * (n - k + i)) / i;
        }
	return (unsigned long)r;
}
#endif

static unsigned long gcd ( unsigned long a, unsigned long b ){
  unsigned long c;
  while ( a != 0 ) {
     c = a; a = b%a;  b = c;
  }
  return b;
}
/* 
   This version using the gcd is a bit slower but avoids the long-long.
   Upper bound of the number of recursions is n/2.
 */
static unsigned long comb(unsigned long n, unsigned long k){
   unsigned long d;
   unsigned long q;
   if(n<k)return 0;
   if(n==k || k == 0)return 1;
   if (k > n/2){
     k = n-k;
   }
   d = gcd( n, k );
   q = k / d;
   return ( comb( n-1, k-1 ) / q ) * ( n / d );
}

static int mp_comb(unsigned long n, unsigned long k,mp_int *c){
	unsigned long i;
        mp_int temp;
        int e;

        if(n<k){
          mp_set_int(c,0);
          return MP_OKAY;
        }
        if(n==k){
          mp_set_int(c,1);
          return MP_OKAY;
        }
        /* That's over-simplified, e.g. comb(99,6) = 1120529256 is still
           smaller than 2^32.
           An upper limit would be (the magic number is 1/sqrt(2*pi))

           ulim = (0.398942280401430*pow(n,(n+.5 ))*pow((n-k),(k-n-.5 ))
                                                          /pow(p,(k + 0.5 )));

           Stanica, Pantelimon. "Good lower and upper bounds on binomial
           coefficients." Journal of Inequalities in Pure and Applied
           Mathematics 2.3 (2001): 30.
         */
        if(n < 35){
          if( (e = mp_set_int(c,comb(n,k)) ) != MP_OKAY){
            return e;
          }
          return MP_OKAY;
        }
        if( (e = mp_set_int(c,1) ) != MP_OKAY){
          return e;
        }
	if (k > n/2){
           k = n-k;
        }
        if( (e = mp_init(&temp) ) != MP_OKAY){
          return e;
        }
        for (i=1; i<=k;i++){
          if( (e = mp_set_int(&temp,(n - k + i) ) ) != MP_OKAY){
            return e;
          }
          if( (e = mp_mul(c,&temp,c) ) != MP_OKAY){
            return e;
          }
          if( (e = mp_set_int(&temp,i) ) != MP_OKAY){
            return e;
          }
          if( (e =  mp_div(c,&temp,c,NULL) ) != MP_OKAY){
            return e;
          }
        }
        mp_clear(&temp);
        return MP_OKAY;
}


int mp_binomial(unsigned long n,unsigned long  k, mp_int *c){
 /*  Idea shamelessly stolen from Calc.
      Hey, wait, I wrote that calc-script myself!
      Oh well, my age's starting to show, I'm afraid ;-)*/
  
  unsigned long *prime_list;
  unsigned long pix=0,prime,K,diff;
  mp_bitset_t *bst;
  int e;

  if(n<k){
    mp_set_int(c,0);
    return MP_OKAY;
  }
  if(n==k || k==0){
    mp_set_int(c,1LU);
    return MP_OKAY;
  }
  if((n-k)==1 || k==1){
    /* TODO: either check for sizeof(unsigned long) == 4 or repair mp_set_int() */
    mp_set_int(c,n);
    return MP_OKAY;
  }

  /* The function mp_comb(n,k) is faster if k<<n (and v.v.), the exact cut-off
     has not been calculated yet but is quite large.*/
  if(k < n/900 || (n-k) < n/900){
    if( (e = mp_comb(n, k, c) ) != MP_OKAY){
      return e;
    }
    return MP_OKAY;
  }

  if (k > n/2){
    k = n-k;
  }

  bst = malloc(sizeof(mp_bitset_t));
  if (bst == NULL) {
      return MP_MEM;
  }
  mp_bitset_alloc(bst, n+1);
  mp_eratosthenes(bst);
  /* One could also count the number of primes in the already filled sieve */
  pix = (unsigned long) (LN_113 *  n/log(n) ) + 2;

  prime_list = malloc(sizeof(unsigned long) * (pix) * 2);
  if (prime_list == NULL) {
      return MP_MEM;
  }
  prime = 2;K=0;
  do{
    diff = mp_prime_divisors(n,prime)-
           ( mp_prime_divisors(n-k,prime)+mp_prime_divisors(k,prime));
    if(diff != 0){
      prime_list[K] = prime;
      prime_list[K+1] = diff;
      K+=2;
     }
    prime          = mp_bitset_nextset(bst,prime+1);
  }while(prime <= k);
  do {
    diff = mp_prime_divisors(n,prime)-mp_prime_divisors(n-k,prime);
    if(diff != 0){
      prime_list[K] = prime;     
      prime_list[K+1] = diff;
      K+=2;
    }
    prime          = mp_bitset_nextset(bst,prime+1);
  }while(prime <= n-k);
  do {
    prime_list[K] = prime;
    prime_list[K+1] = mp_prime_divisors(n,prime);
    prime          = mp_bitset_nextset(bst,prime+1);
    K+=2;
  }while(prime <= n);
  prime_list = realloc(prime_list,sizeof(unsigned long) * K);
  if (prime_list == NULL) {
      return MP_MEM;
  }
  if( (e = mp_compute_factored_factorial(prime_list,K-1,c,0) ) != MP_OKAY){
    return e;
  }
  free(bst);
  free(prime_list);
  return MP_OKAY;
}
#endif
