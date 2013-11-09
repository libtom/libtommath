#include <tommath.h>
#ifdef BN_MP_PRIMORIAL_C
#   include <stdio.h>
#   include <stdlib.h>
#   include <stdint.h>
#   include <string.h>
#   include <inttypes.h>
#   include <limits.h>
#   include <math.h>

/* It's called Hallek's method but it has many inventors*/
static uint32_t isqrt(uint32_t n)
{
    uint32_t s, rem, root;
    if (n < 1)
        return 0;
    /* This is actually the highest square but it goes
     * downward from this, quite fast */
    s = 1 << 30;
    rem = n;
    root = 0;
    while (s > 0) {
        if (rem >= (s | root)) {
            rem -= (s | root);
            root >>= 1;
            root |= s;
        } else {
            root >>= 1;
        }
        s >>= 2;
    }
    return root;
}
uint32_t mp_bitset_nextset(mp_bitset_t * bst, uint32_t n)
{
    while ((n < mp_bitset_size(bst)) && (!mp_bitset_get(bst, n))) {
        n++;
    }
    return n;
}

uint32_t mp_bitset_prevset(mp_bitset_t * bst, uint32_t n)
{
    while (n > 1 && (!mp_bitset_get(bst, n))) {
        n--;
    }
    return n;
}

/* 
 * Standard method, quite antique now, but good enough for the handfull
 * of primes needed here.
 */
void mp_eratosthenes(mp_bitset_t * bst)
{
    uint32_t n, k, r, j;

    mp_bitset_setall(bst);
    mp_bitset_clear(bst, 0);
    mp_bitset_clear(bst, 1);

    n = mp_bitset_size(bst);
    r = isqrt(n);
    for (k = 4; k < n; k += 2)
        mp_bitset_clear(bst, k);
    k = 0;
    while ((k = mp_bitset_nextset(bst, k + 1)) < n) {
        if (k > r) {
            break;
        }
        for (j = k * k; j < n; j += k * 2) {
            mp_bitset_clear(bst, j);
        }
    }
}

#define LN_113 1.25505871293247979696870747618124469168920275806274
unsigned long *mp_fill_prime_list(unsigned long start, unsigned long stop,
                                                          unsigned long *R)
{

    mp_bitset_t *bst;
    uint32_t n, k, j,i;
    unsigned long pix,*list;

    pix = (unsigned long) (LN_113 *  stop/log(stop) ) + 2;

    list = malloc(pix * sizeof(unsigned long));
    if(list == NULL){
      return 0;
    }

    bst = malloc(sizeof(mp_bitset_t));
    mp_bitset_alloc(bst, stop);

    mp_bitset_setall(bst);
    mp_bitset_clear(bst, 0);
    mp_bitset_clear(bst, 1);

    n = mp_bitset_size(bst);
    for (k = 4; k < n; k += 2)
        mp_bitset_clear(bst, k);
    k = 0;
    i = 0;
    while ((k = mp_bitset_nextset(bst, k + 1)) < n) {
      if(k >= start){
         list[i++] = k;
      }
        for (j = k * k; j < n; j += k * 2) {
            mp_bitset_clear(bst, j);

        }
    }
    mp_bitset_free(bst);
    list = realloc(list, sizeof(unsigned long)*(i + 1));
    *R = i;
    return list;
}
#define MP_DIGIT_SIZE (1<<DIGIT_BIT)
#define MP_DIGIT_BIT_HALF (DIGIT_BIT>>1)
#define MP_DIGIT_HALF (1<< MP_DIGIT_BIT_HALF )
#define MP_DIGIT_MASK (MP_DIGIT_HALF-1)
static unsigned long *primelist;
static   mp_int p_temp;
static mp_int *primorial__lowlevel(unsigned long a, unsigned long b ,
                          unsigned long p)
{
  unsigned long  c;
  mp_word prod;
  mp_int ta,tb;
  
  if( b == a){
     mp_set_int(&p_temp,p);
     return  &(p_temp);
  }
  if( b-a > 1){
    c= (b + a) >> 1;
    mp_init_multi(&ta,&tb,NULL);
    mp_copy(primorial__lowlevel( a   , c , primelist[a] ),&ta);
    mp_copy(primorial__lowlevel( c+1 , b , primelist[b] ),&tb);
    if(ta.used == 1 && tb.used == 1){
      if(ta.dp[0] <= MP_DIGIT_HALF || tb.dp[0] <= MP_DIGIT_HALF ){
        prod = (mp_word)ta.dp[0] * tb.dp[0];
        if( prod< MP_DIGIT_SIZE){ 
          mp_set_int(&p_temp,(mp_digit)prod);
          return &p_temp;
        }
      }
    }

      mp_mul(&ta,&tb,&p_temp) ;
      mp_clear_multi(&ta,&tb,NULL);
      return &p_temp;

  }
  if(   (primelist[a]) >=  (MP_DIGIT_HALF ) 
      || (primelist[b]) >=  (MP_DIGIT_HALF )   ){
    mp_set_int( &p_temp,primelist[a]);
    mp_mul_d(&p_temp,primelist[b],&p_temp);
  }
  else{
    c = primelist[a]*primelist[b];
    mp_set_int(&p_temp,c);
  }
  return &p_temp;
}
void mp_primorial(unsigned long a, unsigned long b, mp_int *result){
    unsigned long  r=0; 
    mp_init(&p_temp);
    primelist = mp_fill_prime_list(a+1, b+1, &r);
    primorial__lowlevel(0,r-1,1);
    mp_copy(&p_temp,result);
    mp_clear(&p_temp);
}
#endif
