/* Makes safe primes of a 2k nature */
#include <tommath.h>
#include <time.h>

static int sizes[] = {256, 512, 768, 1024, 1536, 2048, 3072, 4096};

int main(void)
{
   char buf[2000];
   size_t x;
   bool y;
   mp_int q, p;
   mp_err err;
   FILE *out;
   clock_t t1;
   mp_digit z;

   if ((err = mp_init_multi(&q, &p, NULL)) != MP_OKAY)                                                     goto LTM_ERR;

   out = fopen("2kprime.1", "w");
   if (out != NULL) {
      for (x = 0; x < (sizeof(sizes) / sizeof(sizes[0])); x++) {
top:
         if ((err = mp_2expt(&q, sizes[x])) != MP_OKAY)                                                    goto LTM_ERR;
         if ((err = mp_add_d(&q, 3uL, &q)) != MP_OKAY)                                                     goto LTM_ERR;
         z = -3;

         t1 = clock();
         for (;;) {
            if ((err = mp_sub_d(&q, 4uL, &q)) != MP_OKAY)                                                  goto LTM_ERR;
            z += 4uL;

            if (z > MP_MASK) {
               printf("No primes of size %d found\n", sizes[x]);
               break;
            }

            if ((clock() - t1) > CLOCKS_PER_SEC) {
               printf(".");
               fflush(stdout);
               /*            sleep((clock() - t1 + CLOCKS_PER_SEC/2)/CLOCKS_PER_SEC); */
               t1 = clock();
            }

            /* quick test on q */
            if ((err = mp_prime_is_prime(&q, 1, &y)) != MP_OKAY)                                           goto LTM_ERR;
            if (!y) {
               continue;
            }

            /* find (q-1)/2 */
            if ((err = mp_sub_d(&q, 1uL, &p)) != MP_OKAY)                                                  goto LTM_ERR;
            if ((err = mp_div_2(&p, &p)) != MP_OKAY)                                                       goto LTM_ERR;
            if ((err = mp_prime_is_prime(&p, 3, &y)) != MP_OKAY)                                           goto LTM_ERR;
            if (!y) {
               continue;
            }

            /* test on q */
            if ((err = mp_prime_is_prime(&q, 3, &y)) != MP_OKAY)                                           goto LTM_ERR;
            if (!y) {
               continue;
            }

            break;
         }

         if (!y) {
            ++sizes[x];
            goto top;
         }

         if ((err = mp_to_decimal(&q, buf, sizeof(buf))) != MP_OKAY)                                       goto LTM_ERR;
         printf("\n\n%d-bits (k = %lu) = %s\n", sizes[x], z, buf);
         fprintf(out, "%d-bits (k = %lu) = %s\n", sizes[x], z, buf);
         fflush(out);
      }
      fclose(out);
   }
LTM_ERR:
   return 0;
}
