#include <string.h>
#include <time.h>

/*
 * Configuration
 */
#ifndef LTM_DEMO_TEST_VS_MTEST
#define LTM_DEMO_TEST_VS_MTEST 1
#endif

#ifndef LTM_DEMO_TEST_REDUCE_2K_L
/* This test takes a moment so we disable it by default, but it can be:
 * 0 to disable testing
 * 1 to make the test with P = 2^1024 - 0x2A434 B9FDEC95 D8F9D550 FFFFFFFF FFFFFFFF
 * 2 to make the test with P = 2^2048 - 0x1 00000000 00000000 00000000 00000000 4945DDBF 8EA2A91D 5776399B B83E188F
 */
#define LTM_DEMO_TEST_REDUCE_2K_L 0
#endif

#ifdef LTM_DEMO_REAL_RAND
#define LTM_DEMO_RAND_SEED  time(NULL)
#else
#define LTM_DEMO_RAND_SEED  23
#endif

#include "tommath.h"

static void ndraw(mp_int *a, const char *name)
{
   char buf[16000];

   printf("%s: ", name);
   mp_toradix(a, buf, 10);
   printf("%s\n", buf);
   mp_toradix(a, buf, 16);
   printf("0x%s\n", buf);
}

#include "test.h"
#include "opponent.h"

int main(void)
{
#if defined(LTM_DEMO_REAL_RAND)
   if (!fd_urandom) {
      fd_urandom = fopen("/dev/urandom", "r");
      if (!fd_urandom) {
#   if !defined(_WIN32)
         fprintf(stderr, "\ncould not open /dev/urandom\n");
#   endif
      }
   }
#endif
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
   return all_tests();
}

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
