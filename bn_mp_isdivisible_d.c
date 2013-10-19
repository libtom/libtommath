#include <tommath.h>
#ifdef BN_MP_ISDIVISIBLE_D_C
int mp_isdivisible_d(mp_int * a, mp_digit d)
{
    int err;
    mp_digit res;
    if ((err = mp_mod_d(a, d, &res)) != MP_OKAY) {
        return err;
    }
    if (res == 0) {
        return MP_YES;
    }
    return MP_NO;
}
#endif
