#include <tommath.h>
#ifdef BN_MP_SUBFACTORIAL_C

#ifndef MP_DIGIT_SIZE
#define MP_DIGIT_SIZE (1<<DIGIT_BIT)
#endif

#ifdef SAVE_SOME_MEMORY
static int subfactorial_iterative(unsigned long n, mp_int *result){
  mp_int temp1, temp2;
  mp_digit k;
  int e;

  if(n==0){
    return mp_set_int(result,1);
  }
  if(n==1){
    return mp_set_int(result,0);
  }
  if(n==2){
    return mp_set_int(result,1);
  }
  if ( ( e = mp_init_multi(&temp1, &temp2,NULL) ) != MP_OKAY){
    return e;
  }
  if ( ( e = mp_set_int(&temp1, 0) ) != MP_OKAY){
    mp_clear_multi(&temp1, &temp2,NULL);
    return e;
  }
  if ( ( e = mp_set_int(result,1) ) != MP_OKAY){
    mp_clear_multi(&temp1, &temp2,NULL);
    return e;
  }
  for(k=3;k<=n;k++){
    mp_exch ( &temp1,&temp2 );
    mp_exch ( result, &temp1);
   if ( ( e = mp_add(&temp1, &temp2,result))  != MP_OKAY){
     mp_clear_multi(&temp1, &temp2,NULL);
     return e;
   }
   if ( ( e = mp_mul_d(result,(k-1),result))  != MP_OKAY){
     mp_clear_multi(&temp1, &temp2,NULL);
     return e;
   }     
  }
  mp_clear_multi(&temp1, &temp2,NULL);
  return MP_OKAY;
}
#else
static mp_int kill_the_stack;
static int subfactorial_error = MP_OKAY; 
static mp_int * __subfactorialrecursive(unsigned long n){

  mp_digit sign;
  int e;

  if(n==0){
    if ( ( e =  mp_set_int(&kill_the_stack,1) ) != MP_OKAY){
      subfactorial_error = e;
    }
    return &kill_the_stack;
  }
  if(n==1){
    if ( ( e =  mp_set_int(&kill_the_stack,0) ) != MP_OKAY){
      subfactorial_error = e;
    }
    return &kill_the_stack;
  }
  if(n==2){
    if ( ( e =  mp_set_int(&kill_the_stack,1) ) != MP_OKAY){
      subfactorial_error = e;
    }
    return &kill_the_stack;
  }
  if ( ( e =  mp_mul_d(__subfactorialrecursive(n-1),n,&kill_the_stack) ) != MP_OKAY){
      subfactorial_error = e;
  }
  sign = (n&01)?-1:1;
  if ( ( e =  mp_add_d(&kill_the_stack,sign,&kill_the_stack) ) != MP_OKAY){
      subfactorial_error = e;
  }
  return &kill_the_stack;
}
static int subfactorial_recursive(unsigned long n, mp_int *result){
  mp_init(&kill_the_stack);
  subfactorial_error = MP_OKAY;
  __subfactorialrecursive(n);
  mp_copy(&kill_the_stack,result);
  mp_clear(&kill_the_stack);
  return subfactorial_error;
}

#endif

int mp_subfactorial(unsigned long n, mp_int *result){
  if(n > MP_DIGIT_SIZE ){
    return MP_RANGE;
  }
#ifdef SAVE_SOME_MEMORY
   return subfactorial_iterative(n, result);
#else
  return  subfactorial_recursive(n, result);
#endif
}

#endif
