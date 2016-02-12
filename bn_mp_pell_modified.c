#include <tommath.h>
#ifdef BN_MP_PELL_MODIFIED_C
int mp_pell_modified(unsigned long n, mp_int *r)
{
   return mp_fpl_matrix(n, 2,1,1,0 , 1,1,1,1 ,r);
}
#endif
