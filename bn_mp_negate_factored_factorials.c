#include <tommath.h>
#ifdef BN_MP_NEGATE_FACTORED_FACTORIALS_C

int mp_negate_factored_factorials(  long *input,
                                unsigned long l_input,
                                 long **output,
                                unsigned long *l_output){
  unsigned long k, counter=0;
  long neg;

  *output = malloc(sizeof(unsigned long)*(l_input+1));
  if(*output == NULL){
    return MP_MEM;
  }
  for(k=0;k<l_input;k+=2){
    neg = input[k+1];
    if(neg == LONG_MIN){
      return MP_VAL;
    }
    (*output)[counter]   = input[k];
    (*output)[counter+1] = -neg;
    counter += 2;
  }
  *l_output = counter;
  return MP_OKAY;
}

#endif
