#include "shared.h"

int mtest_opponent(void);
int unit_tests(void);

int main(void)
{
   srand(LTM_DEMO_RAND_SEED);

#ifdef MP_8BIT
   printf("Digit size 8 Bit \n");
#endif
#ifdef MP_16BIT
   printf("Digit size 16 Bit \n");
#endif
#ifdef MP_32BIT
   printf("Digit size 32 Bit \n");
#endif
#ifdef MP_64BIT
   printf("Digit size 64 Bit \n");
#endif
   printf("Size of mp_digit: %u\n", (unsigned int)sizeof(mp_digit));
   printf("Size of mp_word: %u\n", (unsigned int)sizeof(mp_word));
   printf("DIGIT_BIT: %d\n", DIGIT_BIT);
   printf("MP_PREC: %d\n", MP_PREC);

   if (LTM_DEMO_TEST_VS_MTEST) {
      return mtest_opponent();
   }
   return unit_tests();
}

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
