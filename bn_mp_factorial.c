#include <tommath.h>
#ifdef BN_MP_FACTORIAL_C

static long highbit(unsigned  long n){
   long r=0;
  while (n >>= 1) {
    r++;
  }
  return r;
}

unsigned long mp_prime_divisors(unsigned long n,unsigned long p){
    unsigned long q,m;
    q = n;
    m = 0LU;
    if (p > n) return 0LU;
    if (p > n/2LU) return 1LU;
    while (q >= p) {
        q  = q/p;
        m += q;
    }
    return m;
}

#define MP_FACTORIAL_BORW_LOOP_CUTOFF 500000
#define MP_FACTORIAL_BORW_PRIMORIAL_CUTOFF 200000
static int factorial_borwein(unsigned long n, mp_int *result){
    unsigned long *p_list;
    unsigned long *exp_list;
    unsigned long p, i,cut;
    long bit;
    int shift;
    mp_int temp;

    unsigned long  r=0;
    p_list = mp_fill_prime_list(3, n+1, &r);
    exp_list = malloc(sizeof(unsigned long)* (r+1));
    if(exp_list == NULL){ return MP_MEM;}

    mp_set_int(result,1);

    shift = (int)mp_prime_divisors(n,2LU);

    cut = n / 2 ;

    for(p = 0; p < r; p++) {
        if(p_list[p] > cut) break;
        exp_list[p] = mp_prime_divisors(n,p_list[p]);
    }
    mp_init(&temp);
    bit = (int)highbit(exp_list[0]);
    if(n < MP_FACTORIAL_BORW_LOOP_CUTOFF){
      for( ; bit>=0; bit--) {
          mp_sqr(result, result);
          for(i=0; i<p; i++) {
              if ((exp_list[i] & (1 << bit)) != 0) {
                  mp_mul_d(result,(mp_digit)p_list[i], result);
              }
          }
      }
    }
    else{
      for( ; bit>=0; bit--) {
          mp_sqr(result, result);
          mp_set_int(&temp,1);
          for(i=0; i<p; i++) {
              if ((exp_list[i] & (1 << bit)) != 0) {
                  mp_mul_d(&temp,(mp_digit)p_list[i], &temp);
              }
          }
          mp_mul(&temp,result,result);
      }
    }
    if(n<MP_FACTORIAL_BORW_PRIMORIAL_CUTOFF){
      for(; p < r; p++){
         mp_mul_d(result,(mp_digit)p_list[p], result);
      }
    }
    else {
      mp_primorial(cut,n,&temp);
      mp_mul(result,&temp,result);
    }
    mp_mul_2d (result, shift, result);
    return MP_OKAY;
}
static mp_int p_temp;
static mp_int *fbinsplit2b(unsigned long n, unsigned long m){
      mp_int t1,t2;
      if (m <= (n + 1)){
          mp_set_int(&p_temp,n);
          return &p_temp;
      }
      if (m == (n + 2)){
          mp_set_int(&p_temp,n * m);
          return &p_temp;
      }
      unsigned long k =  (n + m) / 2;
      if ((k & 1) != 1) k = k - 1;
      mp_init_multi(&t1,&t2,NULL);
      mp_copy(fbinsplit2b(n, k),&t1);
      mp_copy(fbinsplit2b(k + 2, m),&t2);
      mp_mul(&t1 ,&t2 ,&p_temp);
      mp_clear_multi(&t1,&t2,NULL);
      return &p_temp;
}
static void fbinsplit2a(unsigned long n,mp_int *p, mp_int *r){
      if (n <= 2) return;
      fbinsplit2a(n / 2, p, r);
      mp_mul( p , fbinsplit2b(n / 2 + 1 + ((n / 2) & 1), n - 1 + (n & 1)),p);
      mp_mul(r, p,r);
}
static int factorial_binsplit(unsigned long n, mp_int *result){
      mp_int p, r;
      mp_init_multi(&p,&r,NULL);
      mp_set_int(&p,1);
      mp_set_int(&r,1);
      fbinsplit2a(n, &p, &r);
      mp_copy(&r,result);
      mp_mul_2d (&r,(int)mp_prime_divisors(n,2LU) , result);
      return MP_OKAY;
}
#ifndef MP_DIGIT_SIZE
#define MP_DIGIT_SIZE (1<<DIGIT_BIT)
#endif
static int factorial_naive(unsigned long n, mp_int *result){
   unsigned long m,l=1,k=0;
   mp_set_int(result,1);
        for (; n > 1; n--) {
                for (m = n; !(m & 0x1); m >>= 1)k++;
                if (l <= MP_DIGIT_SIZE/m) {
                        l *= m;
                        continue;
                }
                mp_mul_d(result,l, result);
                l = m;
        }
        if (l > 1) {
                mp_mul_d(result, l, result);
        }

  mp_mul_2d (result, k, result);
  return MP_OKAY;
}

int mp_factorial(unsigned long n, mp_int *result){
  switch(n){
    case 0:
    case 1:  return mp_set_int(result,1);
             break;
    case 2:  return mp_set_int(result,2);
             break;
    case 3:  return mp_set_int(result,6);
             break;
    case 4:  return mp_set_int(result,24);
             break;
    case 5:  return mp_set_int(result,120);
             break;
#if (defined MP_16BIT) || (defined MP_28BIT)\
 || (defined MP_31BIT) || (defined MP_64BIT)
    case 6:  return mp_set_int(result,720);
             break;
    case 7:  return mp_set_int(result,5040);
             break;
    case 8:  return mp_set_int(result,40320);
             break;
#endif
#if (defined MP_28BIT)\
 || (defined MP_31BIT)|| (defined MP_64BIT)
    case 9:  return mp_set_int(result,362880LU);
             break;
    case 10: return mp_set_int(result,3628800LU);
             break;
    case 11: return mp_set_int(result,39916800LU);
             break;
#endif
#if (defined MP_31BIT)|| (defined MP_64BIT)
    case 12: return mp_set_int(result,479001600LLU);
             break;
#endif
#if (defined MP_64BIT)
    case 13: return mp_set_int(result,6227020800LLU);
             break;
    case 14: return mp_set_int(result,87178291200LLU);
             break;
    case 15: return mp_set_int(result,1307674368000LLU);
             break;
    case 16: return mp_set_int(result,20922789888000LLU);
             break;
    case 17: return mp_set_int(result,355687428096000LLU);
             break;
    case 18: return mp_set_int(result,6402373705728000LLU);
             break;
    case 19: return mp_set_int(result,121645100408832000LLU);
             break;
#endif
    default: break;
  }

  if(n < 1700){
    return factorial_naive(n,result);
  }
  else if(n < 65536){
    return factorial_binsplit(n,result);  
  }

  return factorial_borwein(n,result);
}

#endif

