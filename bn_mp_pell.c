#include <tommath.h>
#ifdef BN_MP_PELL_C
int mp_pell(unsigned long n, mp_int *r)
{
   return mp_fpl_matrix(n, 2,1,1,0 , 1,0,0,1 ,r);
}
#endif
