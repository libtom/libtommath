#include <tommath.h>
#ifdef BN_MP_EXPT_C
int mp_expt(mp_int *a, mp_int *b, mp_int *c){
  mp_int tmp,e;
  int err;

  /* only positive exponents for now */
  if(b->sign == MP_NEG){
    return MP_VAL;
  }

  if( (err = mp_init_multi(&tmp,&e,NULL) ) != MP_OKAY){
    return err;
  }
  if( (err = mp_copy(a,&tmp) ) != MP_OKAY){
    return err;
  }
  if( (err = mp_copy(b,&e) ) != MP_OKAY){
    return err;
  }
  if( (err = mp_set_int(c,1) ) != MP_OKAY){
    return err;
  }
  while(!mp_iszero(&e)){
    /* could be done more elegantely by checking bit counted down below */
    if(mp_isodd(&e)){
       if( (err = mp_mul(c,&tmp,c) ) != MP_OKAY){
         return err;
       }
    }
    /* could be done more elegantely by counting down highbit */
    if( (err = mp_div_2(&e, &e)) != MP_OKAY){
      return err;
    }
    if(!mp_iszero(&e)){
      if( (err = mp_sqr(&tmp,&tmp)) != MP_OKAY){
        return err;
      }
    }
  }
  if(a->sign == MP_NEG){
    c->sign = (mp_isodd(b))?MP_NEG:MP_ZPOS;
  }
  mp_clear_multi(&tmp,&e,NULL);
  return MP_OKAY;
}
#endif
