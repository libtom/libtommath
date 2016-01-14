#include <tommath.h>
#ifdef BN_MP_MIN_C
/* returns a if equal, min value otherwise */
mp_int *mp_min(mp_int *a, mp_int *b)
{
   int r;
   r = mp_cmp(a,b);
   if (r == MP_EQ ||  r == MP_LT) {
      return a;
   }
   return b;
}
#endif

