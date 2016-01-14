#include <tommath.h>
#ifdef BN_MP_MAX_C
/* returns a if equal, max value otherwise */
mp_int *mp_max(mp_int *a, mp_int *b)
{
   int r;
   r = mp_cmp(a,b);
   if (r == MP_EQ || r == MP_GT) {
      return a;
   }
   return b;
}
#endif
