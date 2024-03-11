/* tests the montgomery routines */
#include <tommath.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
   mp_int modulus, R, p, pp;
   mp_digit mp;
   mp_err err;
   int x, y;

   srand(time(NULL));
   if ((err = mp_init_multi(&modulus, &R, &p, &pp, NULL)) != MP_OKAY)                                      goto LTM_ERR;

   /* loop through various sizes */
   for (x = 4; x < 256; x++) {
      printf("DIGITS == %3d...", x);
      fflush(stdout);

      /* make up the odd modulus */
      if ((err = mp_rand(&modulus, x)) != MP_OKAY)                                                         goto LTM_ERR;
      modulus.dp[0] |= 1uL;

      /* now find the R value */
      if ((err = mp_montgomery_calc_normalization(&R, &modulus)) != MP_OKAY)                               goto LTM_ERR;
      if ((err = mp_montgomery_setup(&modulus, &mp)) != MP_OKAY)                                           goto LTM_ERR;

      /* now run through a bunch tests */
      for (y = 0; y < 1000; y++) {
         /* p = random */
         if ((err = mp_rand(&p, x/2)) != MP_OKAY)                                                          goto LTM_ERR;
         /* pp = R * p */
         if ((err = mp_mul(&p, &R, &pp)) != MP_OKAY)                                                       goto LTM_ERR;
         if ((err = mp_montgomery_reduce(&pp, &modulus, mp)) != MP_OKAY)                                   goto LTM_ERR;

         /* should be equal to p */
         if (mp_cmp(&pp, &p) != MP_EQ) {
            printf("FAILURE!\n");
            exit(-1);
         }
      }
      printf("PASSED\n");
   }

LTM_ERR:
   return 0;
}
