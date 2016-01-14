#include <tommath.h>
#ifdef BN_MP_PELL_LUCAS_C
int mp_pell_lucas(unsigned long n, mp_int *r)
{
   return mp_fpl_matrix(n, 2,1,1,0 , 2,2,2,2 ,r);
}
#endif
