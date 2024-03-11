/* Makes safe primes of a DR nature */
#include <tommath.h>

static int sizes[] = { 1+256/MP_DIGIT_BIT, 1+512/MP_DIGIT_BIT, 1+768/MP_DIGIT_BIT, 1+1024/MP_DIGIT_BIT, 1+2048/MP_DIGIT_BIT, 1+4096/MP_DIGIT_BIT };

int main(void)
{
   bool res;
   int x, y;
   char buf[4096];
   FILE *out;
   mp_int a, b;
   mp_err err;

   if ((err = mp_init(&a)) != MP_OKAY)                                                                     goto LTM_ERR;
   if ((err = mp_init(&b)) != MP_OKAY)                                                                     goto LTM_ERR;

   out = fopen("drprimes.txt", "w");
   if (out != NULL) {
      for (x = 0; x < (int)(sizeof(sizes)/sizeof(sizes[0])); x++) {
top:
         printf("Seeking a %d-bit safe prime\n", sizes[x] * MP_DIGIT_BIT);
         if ((err = mp_grow(&a, sizes[x])) != MP_OKAY)                                                     goto LTM_ERR;
         mp_zero(&a);
         for (y = 1; y < sizes[x]; y++) {
            a.dp[y] = MP_MASK;
         }

         /* make a DR modulus */
         a.dp[0] = -1;
         a.used = sizes[x];

         /* now loop */
         res = false;
         for (;;) {
            a.dp[0] += 4uL;
            if (a.dp[0] >= MP_MASK) break;
            if ((err = mp_prime_is_prime(&a, 1, &res)) != MP_OKAY)                                         goto LTM_ERR;
            if (!res) continue;
            printf(".");
            fflush(stdout);
            if ((err = mp_sub_d(&a, 1uL, &b)) != MP_OKAY)                                                  goto LTM_ERR;
            if ((err = mp_div_2(&b, &b)) != MP_OKAY)                                                       goto LTM_ERR;
            if ((err = mp_prime_is_prime(&b, 3, &res)) != MP_OKAY)                                         goto LTM_ERR;
            if (!res) continue;
            if ((err = mp_prime_is_prime(&a, 3, &res)) != MP_OKAY)                                         goto LTM_ERR;
            if (res) break;
         }

         if (!res) {
            printf("Error not DR modulus\n");
            sizes[x] += 1;
            goto top;
         } else {
            if ((err = mp_to_decimal(&a, buf, sizeof(buf))) != MP_OKAY)                                    goto LTM_ERR;
            printf("\n\np == %s\n\n", buf);
            fprintf(out, "%d-bit prime:\np == %s\n\n", mp_count_bits(&a), buf);
            fflush(out);
         }
      }
      fclose(out);
   }

LTM_ERR:
   mp_clear(&a);
   mp_clear(&b);

   return 0;
}
