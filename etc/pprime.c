/*
 * Generates provable primes
 *
 * Tom St Denis, tomstdenis@gmail.com, http://tom.gmail.com
 *
 */
#include <stdlib.h>
#include <time.h>
#include "../tommath_private.h"

static void mp_print(const char *s, const mp_int *a, int radix, FILE *stream)
{
   mp_err err;
   fputs(s, stream);
   err = mp_fwrite(a, radix, stream);
   if (err != MP_OKAY) {
      fprintf(stderr,"mp_fwrite in mp_print failed. error = %s\n", mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }
   fputc('\n',stream);
}

static mp_digit prime_digit(int bits)
{
   mp_digit d = 0;
   mp_int a;
   mp_err err;

   if ((err = mp_init(&a)) != MP_OKAY)  {
      return 0;
   }

   if ((err = mp_prime_rand(&a, 1, bits, false)) != MP_OKAY)                                               goto LTM_ERR;
   d = a.dp[0];

LTM_ERR:
   mp_clear(&a);
   return d;
}


/* makes a prime of at least k bits */
static mp_err pprime(int k, int li, mp_int *p, mp_int *q)
{
   mp_int  a, b, c, n, x, y, z, v;
   mp_err  err = MP_OKAY;
   int     ii, bits;

   /* single digit ? */
   if (k < (int) MP_DIGIT_BIT) {
      mp_set(p, prime_digit(k));
      if (mp_iszero(p)) {
         return MP_VAL;
      }
      return MP_OKAY;
   }

   if ((err = mp_init_multi(&a, &b, &c, &n, &x, &y, &z, &v, NULL)) != MP_OKAY) {
      return err;
   }

   /* product of first 50 primes */
   if ((err = mp_read_radix(&v, "9NPvy2By/eZ0N6s68ky5K/8UTD0Q7fInhDK9BHnueH92HfzU4+U", 64)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /* set the prime */
   mp_set(&a, prime_digit(MP_DIGIT_BIT));
   if (mp_iszero(&a)) {
      err = MP_VAL;
      goto LTM_ERR;
   }

   /* now loop making the single digit */
   while (mp_count_bits(&a) < k) {
      bits = k - mp_count_bits(&a);
      fprintf(stderr, "prime has %4d bits left\r", bits);
      fflush(stderr);
top:
      if (bits < MP_DIGIT_BIT) {
         mp_set(&b, prime_digit(bits));
      } else {
         mp_set(&b, prime_digit(MP_DIGIT_BIT));
      }
      if (mp_iszero(&b)) {
         err = MP_VAL;
         goto LTM_ERR;
      }

      /* now compute z = a * b * 2 */
      /* z = a * b */
      if ((err = mp_mul(&a, &b, &z)) != MP_OKAY)                                                           goto LTM_ERR;
      /* c = a * b */
      if ((err = mp_copy(&z, &c)) != MP_OKAY)                                                              goto LTM_ERR;
      /* z = 2 * a * b */
      if ((err = mp_mul_2(&z, &z)) != MP_OKAY)                                                             goto LTM_ERR;
      /* n = z + 1 */
      if ((err = mp_add_d(&z, 1uL, &n)) != MP_OKAY)                                                        goto LTM_ERR;
      /* check (n, v) == 1; y = (n, v) */
      if ((err = mp_gcd(&n, &v, &y)) != MP_OKAY)                                                           goto LTM_ERR;

      if (mp_cmp_d(&y, 1uL) != MP_EQ) {
         goto top;
      }
      mp_set(&x, 2u);
      /* now try base x=bases[ii]  */
      for (ii = 0; ii < li; ii++) {
         if ((err = mp_prime_next_prime(&x, -1, false)) != MP_OKAY)                                        goto LTM_ERR;

         /* compute x^a mod n; y = x^a mod n */
         if ((err = mp_exptmod(&x, &a, &n, &y)) != MP_OKAY)                                                goto LTM_ERR;
         /* if y == 1 loop */
         if (mp_cmp_d(&y, 1uL) == MP_EQ) {
            continue;
         }
         /* now x^2a mod n ; y = x^2a mod n*/
         if ((err = mp_sqrmod(&y, &n, &y)) != MP_OKAY)                                                     goto LTM_ERR;

         if (mp_cmp_d(&y, 1uL) == MP_EQ) {
            continue;
         }

         /* compute x^b mod n ; y = x^b mod n*/
         if ((err = mp_exptmod(&x, &b, &n, &y)) != MP_OKAY)                                                goto LTM_ERR;

         /* if y == 1 loop */
         if (mp_cmp_d(&y, 1uL) == MP_EQ) {
            continue;
         }

         /* now x^2b mod n; y = x^2b mod n  */
         if ((err = mp_sqrmod(&y, &n, &y)) != MP_OKAY)                                                     goto LTM_ERR;

         if (mp_cmp_d(&y, 1uL) == MP_EQ) {
            continue;
         }
         /* compute x^c mod n == x^ab mod n ; y = x^ab mod n */
         if ((err = mp_exptmod(&x, &c, &n, &y)) != MP_OKAY)                                                goto LTM_ERR;

         /* if y == 1 loop */
         if (mp_cmp_d(&y, 1uL) == MP_EQ) {
            continue;
         }
         /* now compute (x^c mod n)^2 ; y = x^2ab mod n  */
         if ((err = mp_sqrmod(&y, &n, &y)) != MP_OKAY)                                                     goto LTM_ERR;

         /* y should be 1 */
         if (mp_cmp_d(&y, 1uL) != MP_EQ) {
            continue;
         }
         break;
      }

      /* no bases worked? */
      if (ii == li) {
         goto top;
      }

      mp_print("Certificate of primality for:\n ", &n, 10, stdout);
      mp_print("A == ", &a, 10, stdout);
      mp_print("B == ", &b, 10, stdout);
      mp_print("G == ", &x, 10, stdout);
      printf("----------------------------------------------------------------\n");

      /* a = n */
      if ((err = mp_copy(&n, &a)) != MP_OKAY)                                                              goto LTM_ERR;
   }

   /* get q to be the order of the large prime subgroup */
   if ((err = mp_sub_d(&n, 1uL, q)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_div_2(q, q)) != MP_OKAY)                                                                  goto LTM_ERR;
   if ((err = mp_div(q, &b, q, NULL)) != MP_OKAY)                                                          goto LTM_ERR;

   mp_exch(&n, p);

   err = MP_OKAY;
LTM_ERR:
   mp_clear_multi(&a, &b, &c, &n, &x, &y, &z, &v, NULL);
   return err;
}


int main(void)
{
   mp_int  p, q;
   mp_err err;
   char    buf[4096];
   int     k, li;
   clock_t t1;

   printf("Enter # of bits: \n");
   fgets(buf, sizeof(buf), stdin);
   sscanf(buf, "%d", &k);

   printf("Enter number of bases to try\n");
   fgets(buf, sizeof(buf), stdin);
   sscanf(buf, "%d", &li);


   if ((err = mp_init_multi(&p, &q, NULL)) != MP_OKAY)                                                     goto LTM_ERR;

   t1 = clock();
   if ((err = pprime(k, li, &p, &q)) != MP_OKAY) {
      fprintf(stderr, "Something went wrong in function pprime: %s\n", mp_error_to_string(err));
      goto LTM_ERR;
   }
   t1 = clock() - t1;

   printf("\n\nTook %lu ticks, %d bits\n", t1, mp_count_bits(&p));

   mp_print("P == ", &p, 10, stdout);
   mp_print("Q == ", &q, 10, stdout);

   mp_clear_multi(&p, &q, NULL);
   exit(EXIT_SUCCESS);
LTM_ERR:
   mp_clear_multi(&p, &q, NULL);
   exit(EXIT_FAILURE);
}
