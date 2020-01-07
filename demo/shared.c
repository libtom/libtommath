#include "shared.h"

void ndraw(const mp_int *a, const char *name)
{
   char *buf;
   size_t size = 0;
   mp_err err;

   if ((err = mp_radix_size_overestimate(a, 10, &size)) != MP_OKAY) {
      fprintf(stderr, "\nndraw: mp_radix_size_overestimate(a, 10, %zu) failed - %s\n", size, mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }
   buf = (char *)malloc(size);
   if (buf == NULL) {
      fprintf(stderr, "\nndraw: malloc(%zu) failed\n", size);
      exit(EXIT_FAILURE);
   }

   printf("%s: ", name);
   if ((err = mp_to_decimal(a, buf, size)) != MP_OKAY) {
      fprintf(stderr, "\nndraw: mp_to_decimal(a, buf, %zu) failed - %s\n", size, mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }
   printf("%s\n", buf);
   if ((err = mp_to_hex(a, buf, size)) != MP_OKAY) {
      fprintf(stderr, "\nndraw: mp_to_hex(a, buf, %zu) failed - %s\n", size, mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }
   printf("0x%s\n", buf);

   free(buf);
}

void print_header(void)
{
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
   printf("MP_DEFAULT_DIGIT_COUNT: %d\n", MP_DEFAULT_DIGIT_COUNT);
}
