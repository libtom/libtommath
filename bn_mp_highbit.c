#include <tommath.h>
#ifdef BN_MP_HIGHBIT_C
/* for convenience */
int mp_highbit(mp_int *a)
{
   /* counting from zero such that highbit == lowbit for x = 2^n ; n>=0 */
   return mp_count_bits(a) - 1;
}
#endif
