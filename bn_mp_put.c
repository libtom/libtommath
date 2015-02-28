#include <tommath.h>
#ifdef BN_MP_PUT_C


int mp_put(mp_int * a, int base)
{
   return mp_fput(a, base, stdout);
}

#endif
