#include <tommath.h>
#ifdef BN_MP_ISGE32B_C
int mp_isge32b(mp_int * a)
{
    if (a->used == 1) {
#ifdef MP_64BIT
      if(a->dp[0] >= 0xffffffffllu){
           return MP_YES;
      }
#endif
      return MP_NO;
    }
    return MP_YES;
}
#endif
