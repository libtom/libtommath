#include <tommath.h>
#ifdef BN_MP_ISGE32B_C
int mp_isge32b(mp_int * a)
{
    if (a->used == 1) {
        if (DIGIT_BIT >= 32) {
            if(a->dp[0] >= ((1LLU)<<32)){
              return MP_YES;
            }
        }
        return MP_NO;
    }
    return MP_YES;
}
#endif
