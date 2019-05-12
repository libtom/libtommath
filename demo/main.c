#include "shared.h"

int mtest_opponent(void);
int unit_tests(int argc, char **argv);

void ndraw(mp_int *a, const char *name)
{
   char *buf;
   int size;

   mp_radix_size(a, 10, &size);
   buf = (char *)malloc((size_t) size);
   if (buf == NULL) {
      fprintf(stderr, "\nndraw: malloc(%d) failed\n", size);
      exit(EXIT_FAILURE);
   }

   printf("%s: ", name);
   mp_toradix(a, buf, 10);
   printf("%s\n", buf);
   mp_toradix(a, buf, 16);
   printf("0x%s\n", buf);

   free(buf);
}

int main(int argc, char **argv)
{
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
   printf("MP_DIGIT_BIT: %d\n", MP_DIGIT_BIT);
   printf("MP_PREC: %d\n", MP_PREC);

   if (LTM_DEMO_TEST_VS_MTEST) {
      return mtest_opponent();
   }
   return unit_tests(argc, argv);
}
