#include <tommath.h>
#ifdef BN_MP_SUPERFACTORIAL_C
int mp_superfactorial(unsigned long n, mp_int *c)
{
   long *t, *s;
   unsigned long i, length_t, length_s;
   int e;
   if (n < 2) {
      mp_set(c, 1);
      return MP_OKAY;
   }
   if (n == 2) {
      mp_set(c, 2);
      return MP_OKAY;
   }
   if ((e = mp_factor_factorial(2, 0, &s, &length_s)) != MP_OKAY) {
      free(s);
      return e;
   }
   for (i = 3; i < n; i++) {
      if ((e = mp_factor_factorial(i, 1, &t, &length_t)) != MP_OKAY) {
         free(s);
         free(t);
         return e;
      }
      if ((e =
              mp_add_factored_factorials(s, length_s, t, length_t, &s,
                                         &length_s)) != MP_OKAY) {
         free(s);
         free(t);
         return e;
      }
      free(t);
      t = NULL;
      length_t = 0;
   }
   if ((e =
           mp_compute_signed_factored_factorials(s, length_s, c,
                 NULL)) != MP_OKAY) {
      free(s);
      return e;
   }
   free(s);
   return MP_OKAY;
}
#endif
