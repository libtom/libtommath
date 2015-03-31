#include <tommath.h>
#ifdef BN_MP_COMPUTE_SIGNED_FACTORED_FACTORIALS_C
static long local_highbit(unsigned  long n){
   long r=0;
  while (n >>= 1) {
    r++;
  }
  return r;
}
/*
  To compute the resulting polynomials we need an approach different from
  mp_compute_factored_factorials() because we have to be able to handle negative
  exponents, too.
*/
int mp_compute_signed_factored_factorials(long *f, unsigned long f_length,
                                    mp_int *c, mp_int *r){
  unsigned long start=0,i,count=0,neg_count = 0,k=0;
  long shift = 0;
  long bit=0,neg_bit=0,hb=0,neg_hb=0,*p=NULL;
  mp_int temp;
  int e;

  if(f[0] == 2){
    shift = f[1];
    if(f_length == 2){
      if(shift > 0){
        if( (e = mp_set_int(c,1LU<<(unsigned long)f[1]) ) != MP_OKAY){
         return e;
        }
      }
      return MP_OKAY;
    }
  }
  if(shift){
    start+=2;
    k=2;
  }
  /* 
    One simple thing to handle negative exponents is to keep them in a second
    array with absolute values of the exponents and divide at the end.
    Another alternative might be to return both arrays and let the user decide.
    No matter which way, we have to check and if there is at least one, divide.

    This is expensive and should be done inline instead but for now...
   */
  for(k=start;k<f_length;k+=2){
    if(f[k+1] < 0){
      /* count how many to save some memory */
      count++;
    }
    /* Only primes up to 2^DIGIT_BIT because of mp_mul_d() */
    if(f[k] >= 1L<<DIGIT_BIT){
      return MP_VAL;
    }
  }
  /* all are negative */
  //if(count && count == f_length/2){
    /* if the alternative with two outputs has been chosen just skip the
       computation of the polynomial with all positive exponents and set
       that return to 1 (one)
     */
    /* goto: negative_exponents; */
    
    /* The other alternative would compute 1/x that gives always 0 in integer
       divisions if x>1. But it would need exactly one exponent with zero which
       has been filtered out already. It is debatable now if that was a good
       decision.*/
    //return MP_OKAY;
  //}

  if(count){
    p = malloc( sizeof(long)* (count * 2) +1);
    if(p == NULL){
      return MP_MEM;
    }
  }

  for(k=start;k<f_length;k+=2){
    /* won't branch if count == 0 */
    if(f[k+1] < 0){
      p[neg_count]   = f[k];
      p[neg_count+1] = abs(f[k+1]);
      neg_count +=2 ;
      /* And while we are already here ... */
      neg_hb = local_highbit(abs(f[k+1]));
      if(neg_bit < neg_hb)neg_bit = neg_hb;
      /* This allows for some optimization, mainly w.r.t memory. Not done yet */
      f[k]   = 1;
      /* Set to zero to let the main loop skip it */
      f[k+1] = 0;
    }
    else{
      hb = local_highbit(f[k+1]);
      if(bit < hb)bit = hb;
    }
  }

  /* DIGIT_BIT can be as small as 8 */
  if(bit >(long) DIGIT_BIT || neg_bit >(long) DIGIT_BIT){
    return MP_VAL;
  }
  /* Anything times zero is zero, so set the result to one in advance */
  if( (e = mp_set_int(c,1) ) != MP_OKAY){
    return e;
  }

  /* Everything is in place, so let's start by computing with the positive
     exponents first */

  /* The other function mp_compute_factored_factorials() has a branch
     but the cases where it makes sense are quite rare. Feel free to add it
     yourself */
  for( ; bit>=0; bit--) {
      if( (e = mp_sqr(c, c) ) != MP_OKAY){
        return e;
      }
      for(i=start; i<f_length; i+=2) {
          if ((f[i+1] & (1LU << (unsigned long)bit)) != 0) {
              /* This is problematic if DIGIT_BIT is too small. checked above */
              if( (e = mp_mul_d(c,(mp_digit)f[i], c) ) != MP_OKAY){
                return e;
              }
          }
      }
  }

  /* Compute c * 2^n if n!=0 */
  if(shift && shift > 0){
    if(shift < 1L<<DIGIT_BIT){
      /* The choice of types is a bit questionable. Sometimes. */
      if( (e = mp_mul_2d (c, (mp_digit) shift, c) ) != MP_OKAY){
        return e;
      }
    }
    else{
      long multiplicator = 0;
another_round:
      while(shift > 1L<<DIGIT_BIT){
        shift >>= 1;
        multiplicator++;
      }
      if( (e = mp_mul_2d (c, (mp_digit) shift, c) ) != MP_OKAY){
        return e;
      }
      if(multiplicator< DIGIT_BIT){
        if( (e = mp_mul_2d (c, (mp_digit)(1<<multiplicator), c) ) != MP_OKAY){
          return e;
        }
      }
      else{
        shift = 1<<multiplicator;
        multiplicator = 0;
        goto another_round;
      }
    }
  }

  /* None are negative*/
  if(count == 0){
    /* Clean up */
    /* Nothing to clean up */
    return MP_OKAY;
  }

  /* Compute with the negative eponents */
  if( (e = mp_init(&temp) ) != MP_OKAY){
    return e;
  }
  if( (e = mp_set_int(&temp,1) ) != MP_OKAY){
    return e;
  }
  for( ; neg_bit>=0; neg_bit--) {
      if( (e = mp_sqr(&temp, &temp) ) != MP_OKAY){
        return e;
      }
      /* The exponents of 2 hve been stripped already so we start at zero.
         "count" counts only the occurences, the size itself is twice as large.
       */
      for(i=0; i<count*2; i+=2) {
          if ((p[i+1] & (1LU << (unsigned long)neg_bit)) != 0) {
              if( (e = mp_mul_d(&temp,(mp_digit)p[i], &temp) ) != MP_OKAY){
                return e;
              }
          }
      }
  }

  /* Compute c * 2^n if n!=0  for negative n*/
  if(shift && shift < 0){
    /* check for overflow */
    if(shift == LONG_MIN){
        return MP_VAL;
    }
    shift = -shift;
    if(shift < 1L<<DIGIT_BIT){
      /* The choice of types is a bit questionable. Sometimes. */
      if( (e = mp_mul_2d (&temp, (mp_digit) shift, &temp) ) != MP_OKAY){
        return e;
      }
    }
    else{
      long multiplicator = 0;
and_another_round:
      while(shift > 1L<<DIGIT_BIT){
        shift >>= 1;
        multiplicator++;
      }
      if( (e = mp_mul_2d (&temp, (mp_digit) shift, &temp) ) != MP_OKAY){
        return e;
      }
      if(multiplicator< DIGIT_BIT){
        if( (e = mp_mul_2d (&temp, (mp_digit)(1<<multiplicator), &temp) ) != MP_OKAY){
          return e;
        }
      }
      else{
        shift = 1<<multiplicator;
        multiplicator = 0;
        goto and_another_round;
      }
    }
  }
/*
  Another alternative would be to check for "r == NULL" which wouldn't be
  such a strictly compile-time-only decision
*/
#ifdef BN_MP_DO_LAST_DIVISION
  if( (e = mp_div(c,&temp,c,r) ) != MP_OKAY){
    return e;
  }
#else
  if(r != NULL){
    if( (e = mp_copy(&temp,r) ) != MP_OKAY){
      return e;
    }
  }
#endif
  free(p);
  mp_clear(&temp);
  return MP_OKAY;
}

#endif
