#include <tommath.h>
#ifdef BN_MP_ADD_FACTORED_FACTORIALS_C
int mp_add_factored_factorials(  long *summand_1,
                                unsigned long l_summand_1,
                                 long *summand_2,
                                unsigned long l_summand_2,
                                 long **sum,
                                unsigned long *l_sum){
  unsigned long k, counter=0;
  unsigned long max_length = MAX(l_summand_1,l_summand_2);
  unsigned long min_length = MIN(l_summand_1,l_summand_2);
  long s,p, s_1,s_2;
  /* For more detailed comments see mp_subtract_factored_factorials() */
  *sum = malloc(sizeof(unsigned long)*(max_length+1));
  if(*sum == NULL){
    return MP_MEM;
  }
  for(k=0;k<min_length;k+=2){
    p = summand_1[k];
    /* Over/underflow possible! */
    /*s = summand_1[k+1] + summand_2[k+1];*/
    s_1 = summand_1[k+1];
    s_2 = summand_2[k+1];

    if((s_2 > 0) && (s_1 > LONG_MAX - s_2)){
      /* overflow */
      return MP_VAL;
    }
    if((s_2 < 0) && (s_1 < LONG_MIN - s_2)){
      /* underflow */
      return MP_VAL;
    }
    s = s_1 + s_2;
    (*sum)[counter]   = p;
    (*sum)[counter+1] = s;
    counter += 2;
  }
  if(l_summand_1 >= l_summand_2){
    for(k=0;k<max_length;k+=2){
      p = summand_1[k];
      s = summand_1[k+1];
      (*sum)[counter]   = p;
      (*sum)[counter+1] = s;
      counter += 2;
    } 
  }
  else{
    for(k=0;k<max_length;k+=2){
      p = summand_2[k];
      s = summand_2[k+1];
      (*sum)[counter]   = p;
      (*sum)[counter+1] = s;
      counter += 2;
    } 
  }
  *l_sum = counter;
  return MP_OKAY;
}

#endif
