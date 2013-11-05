#include <tommath.h>
#ifdef BN_MP_POWER_FACTORED_FACTORIALS_C

int mp_power_factored_factorials(  long *input,
                                unsigned long l_input,
                                 long multiplicator,
                                 long **product,
                                unsigned long *l_product){

  unsigned long k, counter=0;

  long prod,p, p_1,p_2;
#ifdef MP_USE_LONG_LONG_AND_HOPE_FOR_THE_BEST
  long long temp;
#endif
  *product = malloc(sizeof(unsigned long)*(l_input+1));
  if(*product == NULL){
    return MP_MEM;
  }
  p_2 = multiplicator;
  for(k=0;k<l_input;k+=2){
    p = input[k];
    /* Over/underflow possible! */
    /*prod = input[k+1] * multiplicator;*/
    p_1 = input[k+1];
    /* Two alternatives: use "long long" and hope for the best or do it the
       hard way */
#ifdef MP_USE_LONG_LONG_AND_HOPE_FOR_THE_BEST
    temp = p * (long long)p_1;
    if ((temp > LONG_MAX) || (tmp < LONG_MIN)) {
      return MP_VAL;
    }
    prod = (long) temp;
#else
    if (p > 0) {
      if (p_1 > 0) {
        if (p > (LONG_MAX / p_1)) {
          return MP_VAL;
        }
      }
      else {
        if (p_1 < (LONG_MIN / p)) {
          return MP_VAL;
        }
      }
    } 
    else {
      if (p_1 > 0) {
        if (p < (LONG_MIN / p_1)) {
          return MP_VAL;
        }
      }
       else {
        if ( (p != 0) && (p_1 < (LONG_MAX / p))) {
          return MP_VAL;
        }
      }
    }
    prod = p * p_1;
#endif
    (*product)[counter]   = p;
    (*product)[counter+1] = prod;
    counter += 2;
  }

  *l_product = counter;
  return MP_OKAY;
}

#endif
