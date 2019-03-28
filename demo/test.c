#include "shared.h"

static int test_trivial_stuff(void)
{
   mp_int a, b, c, d;
   if (mp_init_multi(&a, &b, &c, &d, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* a: 0->5 */
   mp_set_int(&a, 5uL);
   /* a: 5-> b: -5 */
   mp_neg(&a, &b);
   if (mp_cmp(&a, &b) != MP_GT) {
      goto LBL_ERR;
   }
   if (mp_cmp(&b, &a) != MP_LT) {
      goto LBL_ERR;
   }
   /* a: 5-> a: -5 */
   mp_neg(&a, &a);
   if (mp_cmp(&b, &a) != MP_EQ) {
      goto LBL_ERR;
   }
   /* a: -5-> b: 5 */
   mp_abs(&a, &b);
   if (mp_isneg(&b) != MP_NO) {
      goto LBL_ERR;
   }
   /* a: -5-> b: -4 */
   mp_add_d(&a, 1uL, &b);
   if (mp_isneg(&b) != MP_YES) {
      goto LBL_ERR;
   }
   if (mp_get_int(&b) != 4) {
      goto LBL_ERR;
   }
   /* a: -5-> b: 1 */
   mp_add_d(&a, 6uL, &b);
   if (mp_get_int(&b) != 1) {
      goto LBL_ERR;
   }
   /* a: -5-> a: 1 */
   mp_add_d(&a, 6uL, &a);
   if (mp_get_int(&a) != 1) {
      goto LBL_ERR;
   }
   mp_zero(&a);
   /* a: 0-> a: 6 */
   mp_add_d(&a, 6uL, &a);
   if (mp_get_int(&a) != 6) {
      goto LBL_ERR;
   }

   mp_set_int(&a, 42uL);
   mp_set_int(&b, 1uL);
   mp_neg(&b, &b);
   mp_set_int(&c, 1uL);
   mp_exptmod(&a, &b, &c, &d);

   mp_set_int(&c, 7uL);
   mp_exptmod(&a, &b, &c, &d);

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int test_mp_jacobi(void)
{
   struct mp_jacobi_st {
      unsigned long n;
      int c[16];
   };

   static struct mp_jacobi_st jacobi[] = {
      { 3, {  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1 } },
      { 5, {  0,  1, -1, -1,  1,  0,  1, -1, -1,  1,  0,  1, -1, -1,  1,  0 } },
      { 7, {  1, -1,  1, -1, -1,  0,  1,  1, -1,  1, -1, -1,  0,  1,  1, -1 } },
      { 9, { -1,  1,  0,  1,  1,  0,  1,  1,  0,  1,  1,  0,  1,  1,  0,  1 } },
   };

   int i, n, err, should, cnt;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   mp_set_int(&a, 0uL);
   mp_set_int(&b, 1uL);
   if ((err = mp_jacobi(&a, &b, &i)) != MP_OKAY) {
      printf("Failed executing mp_jacobi(0 | 1) %s.\n", mp_error_to_string(err));
      goto LBL_ERR;
   }
   if (i != 1) {
      printf("Failed trivial mp_jacobi(0 | 1) %d != 1\n", i);
      goto LBL_ERR;
   }
   for (cnt = 0; cnt < (int)(sizeof(jacobi)/sizeof(jacobi[0])); ++cnt) {
      mp_set_int(&b, jacobi[cnt].n);
      /* only test positive values of a */
      for (n = -5; n <= 10; ++n) {
         mp_set_int(&a, abs(n));
         should = MP_OKAY;
         if (n < 0) {
            mp_neg(&a, &a);
            /* Until #44 is fixed the negative a's must fail */
            should = MP_VAL;
         }
         if ((err = mp_jacobi(&a, &b, &i)) != should) {
            printf("Failed executing mp_jacobi(%d | %lu) %s.\n", n, jacobi[cnt].n, mp_error_to_string(err));
            goto LBL_ERR;
         }
         if ((err == MP_OKAY) && (i != jacobi[cnt].c[n + 5])) {
            printf("Failed trivial mp_jacobi(%d | %lu) %d != %d\n", n, jacobi[cnt].n, i, jacobi[cnt].c[n + 5]);
            goto LBL_ERR;
         }
      }
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_kronecker(void)
{
   struct mp_kronecker_st {
      long n;
      int c[21];
   };
   static struct mp_kronecker_st kronecker[] = {
      /*-10, -9, -8, -7,-6, -5, -4, -3, -2, -1, 0, 1,  2,  3, 4,  5,  6,  7,  8, 9, 10*/
      { -10, {  0, -1,  0, -1, 0,  0,  0,  1,  0, -1, 0, 1,  0, -1, 0,  0,  0,  1,  0, 1,  0  } },
      {  -9, { -1,  0, -1,  1, 0, -1, -1,  0, -1, -1, 0, 1,  1,  0, 1,  1,  0, -1,  1, 0,  1  } },
      {  -8, {  0, -1,  0,  1, 0,  1,  0, -1,  0, -1, 0, 1,  0,  1, 0, -1,  0, -1,  0, 1,  0  } },
      {  -7, {  1, -1, -1,  0, 1,  1, -1,  1, -1, -1, 0, 1,  1, -1, 1, -1, -1,  0,  1, 1, -1  } },
      {  -6, {  0,  0,  0, -1, 0, -1,  0,  0,  0, -1, 0, 1,  0,  0, 0,  1,  0,  1,  0, 0,  0  } },
      {  -5, {  0, -1,  1, -1, 1,  0, -1, -1,  1, -1, 0, 1, -1,  1, 1,  0, -1,  1, -1, 1,  0  } },
      {  -4, {  0, -1,  0,  1, 0, -1,  0,  1,  0, -1, 0, 1,  0, -1, 0,  1,  0, -1,  0, 1,  0  } },
      {  -3, { -1,  0,  1, -1, 0,  1, -1,  0,  1, -1, 0, 1, -1,  0, 1, -1,  0,  1, -1, 0,  1  } },
      {  -2, {  0, -1,  0,  1, 0,  1,  0, -1,  0, -1, 0, 1,  0,  1, 0, -1,  0, -1,  0, 1,  0  } },
      {  -1, { -1, -1, -1,  1, 1, -1, -1,  1, -1, -1, 1, 1,  1, -1, 1,  1, -1, -1,  1, 1,  1  } },
      {   0, {  0,  0,  0,  0, 0,  0,  0,  0,  0,  1, 0, 1,  0,  0, 0,  0,  0,  0,  0, 0,  0  } },
      {   1, {  1,  1,  1,  1, 1,  1,  1,  1,  1,  1, 1, 1,  1,  1, 1,  1,  1,  1,  1, 1,  1  } },
      {   2, {  0,  1,  0,  1, 0, -1,  0, -1,  0,  1, 0, 1,  0, -1, 0, -1,  0,  1,  0, 1,  0  } },
      {   3, {  1,  0, -1, -1, 0, -1,  1,  0, -1,  1, 0, 1, -1,  0, 1, -1,  0, -1, -1, 0,  1  } },
      {   4, {  0,  1,  0,  1, 0,  1,  0,  1,  0,  1, 0, 1,  0,  1, 0,  1,  0,  1,  0, 1,  0  } },
      {   5, {  0,  1, -1, -1, 1,  0,  1, -1, -1,  1, 0, 1, -1, -1, 1,  0,  1, -1, -1, 1,  0  } },
      {   6, {  0,  0,  0, -1, 0,  1,  0,  0,  0,  1, 0, 1,  0,  0, 0,  1,  0, -1,  0, 0,  0  } },
      {   7, { -1,  1,  1,  0, 1, -1,  1,  1,  1,  1, 0, 1,  1,  1, 1, -1,  1,  0,  1, 1, -1  } },
      {   8, {  0,  1,  0,  1, 0, -1,  0, -1,  0,  1, 0, 1,  0, -1, 0, -1,  0,  1,  0, 1,  0  } },
      {   9, {  1,  0,  1,  1, 0,  1,  1,  0,  1,  1, 0, 1,  1,  0, 1,  1,  0,  1,  1, 0,  1  } },
      {  10, {  0,  1,  0, -1, 0,  0,  0,  1,  0,  1, 0, 1,  0,  1, 0,  0,  0, -1,  0, 1,  0  } }
   };

   long k, m;
   int i, err, cnt;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   mp_set_int(&a, 0uL);
   mp_set_int(&b, 1uL);
   if ((err = mp_kronecker(&a, &b, &i)) != MP_OKAY) {
      printf("Failed executing mp_kronecker(0 | 1) %s.\n", mp_error_to_string(err));
      goto LBL_ERR;
   }
   if (i != 1) {
      printf("Failed trivial mp_kronecker(0 | 1) %d != 1\n", i);
      goto LBL_ERR;
   }
   for (cnt = 0; cnt < (int)(sizeof(kronecker)/sizeof(kronecker[0])); ++cnt) {
      k = kronecker[cnt].n;
      if (k < 0) {
         mp_set_int(&a, (unsigned long)(-k));
         mp_neg(&a, &a);
      } else {
         mp_set_int(&a, (unsigned long) k);
      }
      /* only test positive values of a */
      for (m = -10; m <= 10; m++) {
         if (m < 0) {
            mp_set_int(&b,(unsigned long)(-m));
            mp_neg(&b, &b);
         } else {
            mp_set_int(&b, (unsigned long) m);
         }
         if ((err = mp_kronecker(&a, &b, &i)) != MP_OKAY) {
            printf("Failed executing mp_kronecker(%ld | %ld) %s.\n", kronecker[cnt].n, m, mp_error_to_string(err));
            goto LBL_ERR;
         }
         if ((err == MP_OKAY) && (i != kronecker[cnt].c[m + 10])) {
            printf("Failed trivial mp_kronecker(%ld | %ld) %d != %d\n", kronecker[cnt].n, m, i, kronecker[cnt].c[m + 10]);
            goto LBL_ERR;
         }
      }
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_complement(void)
{
   int i;

   mp_int a, b, c;
   if (mp_init_multi(&a, &b, &c, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < 1000; ++i) {
      int l = (rand() * rand() + 1) * (rand() % 1 ? -1 : 1);
      mp_set_int(&a, labs(l));
      if (l < 0)
         mp_neg(&a, &a);
      mp_complement(&a, &b);

      l = ~l;
      mp_set_int(&c, labs(l));
      if (l < 0)
         mp_neg(&c, &c);

      if (mp_cmp(&b, &c) != MP_EQ) {
         printf("\nmp_complement() bad result!");
         goto LBL_ERR;
      }
   }

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_tc_div_2d(void)
{
   int i;

   mp_int a, b, d;
   if (mp_init_multi(&a, &b, &d, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < 1000; ++i) {
      int l, em;

      l = (rand() * rand() + 1) * (rand() % 1 ? -1 : 1);
      mp_set_int(&a, labs(l));
      if (l < 0)
         mp_neg(&a, &a);

      em = rand() % 32;

      mp_set_int(&d, labs(l >> em));
      if ((l >> em) < 0)
         mp_neg(&d, &d);

      mp_tc_div_2d(&a, em, &b);
      if (mp_cmp(&b, &d) != MP_EQ) {
         printf("\nmp_tc_div_2d() bad result!");
         goto LBL_ERR;
      }
   }

   mp_clear_multi(&a, &b, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &d, NULL);
   return EXIT_FAILURE;

}

static int test_mp_tc_xor(void)
{
   int i;

   mp_int a, b, c, d;
   if (mp_init_multi(&a, &b, &c, &d, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < 1000; ++i) {
      int l, em;

      l = (rand() * rand() + 1) * (rand() % 1 ? -1 : 1);
      mp_set_int(&a, labs(l));
      if (l < 0)
         mp_neg(&a, &a);

      em = (rand() * rand() + 1) * (rand() % 1 ? -1 : 1);
      mp_set_int(&b, labs(em));
      if (em < 0)
         mp_neg(&b, &b);

      mp_set_int(&d, labs(l ^ em));
      if ((l ^ em) < 0)
         mp_neg(&d, &d);

      mp_tc_xor(&a, &b, &c);
      if (mp_cmp(&c, &d) != MP_EQ) {
         printf("\nmp_tc_xor() bad result!");
         goto LBL_ERR;
      }
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;

}

static int test_mp_tc_or(void)
{
   int i;

   mp_int a, b, c, d;
   if (mp_init_multi(&a, &b, &c, &d, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < 1000; ++i) {
      int l, em;

      l = (rand() * rand() + 1) * (rand() % 1 ? -1 : 1);
      mp_set_int(&a, labs(l));
      if (l < 0)
         mp_neg(&a, &a);

      em = (rand() * rand() + 1) * (rand() % 1 ? -1 : 1);
      mp_set_int(&b, labs(em));
      if (em < 0)
         mp_neg(&b, &b);

      mp_set_int(&d, labs(l | em));
      if ((l | em) < 0)
         mp_neg(&d, &d);

      mp_tc_or(&a, &b, &c);
      if (mp_cmp(&c, &d) != MP_EQ) {
         printf("\nmp_tc_or() bad result!");
         goto LBL_ERR;
      }
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int test_mp_tc_and(void)
{
   int i;

   mp_int a, b, c, d;
   if (mp_init_multi(&a, &b, &c, &d, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < 1000; ++i) {
      int l, em;

      l = (rand() * rand() + 1) * (rand() % 1 ? -1 : 1);
      mp_set_int(&a, labs(l));
      if (l < 0)
         mp_neg(&a, &a);

      em = (rand() * rand() + 1) * (rand() % 1 ? -1 : 1);
      mp_set_int(&b, labs(em));
      if (em < 0)
         mp_neg(&b, &b);

      mp_set_int(&d, labs(l & em));
      if ((l & em) < 0)
         mp_neg(&d, &d);

      mp_tc_and(&a, &b, &c);
      if (mp_cmp(&c, &d) != MP_EQ) {
         printf("\nmp_tc_and() bad result!");
         goto LBL_ERR;
      }
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int test_mp_invmod(void)
{
   mp_int a, b, c, d;
   if (mp_init_multi(&a, &b, &c, &d, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* mp_invmod corner-case of https://github.com/libtom/libtommath/issues/118 */
   {
      const char *a_ = "47182BB8DF0FFE9F61B1F269BACC066B48BA145D35137D426328DC3F88A5EA44";
      const char *b_ = "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF";
      const char *should_ = "0521A82E10376F8E4FDEF9A32A427AC2A0FFF686E00290D39E3E4B5522409596";

      if (mp_read_radix(&a, a_, 16) != MP_OKAY) {
         printf("\nmp_read_radix(a) failed!");
         goto LBL_ERR;
      }
      if (mp_read_radix(&b, b_, 16) != MP_OKAY) {
         printf("\nmp_read_radix(b) failed!");
         goto LBL_ERR;
      }
      if (mp_read_radix(&c, should_, 16) != MP_OKAY) {
         printf("\nmp_read_radix(should) failed!");
         goto LBL_ERR;
      }

      if (mp_invmod(&a, &b, &d) != MP_OKAY) {
         printf("\nmp_invmod() failed!");
         goto LBL_ERR;
      }

      if (mp_cmp(&c, &d) != MP_EQ) {
         printf("\nmp_invmod() bad result!");
         goto LBL_ERR;
      }
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;

}

static int test_mp_set_double(void)
{
   int i;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* test mp_get_double/mp_set_double */
#if defined(__STDC_IEC_559__) || defined(__GCC_IEC_559)
   if (mp_set_double(&a, +1.0/0.0) != MP_VAL) {
      printf("\nmp_set_double should return MP_VAL for +inf");
      goto LBL_ERR;
   }
   if (mp_set_double(&a, -1.0/0.0) != MP_VAL) {
      printf("\nmp_set_double should return MP_VAL for -inf");
      goto LBL_ERR;
   }
   if (mp_set_double(&a, +0.0/0.0) != MP_VAL) {
      printf("\nmp_set_double should return MP_VAL for NaN");
      goto LBL_ERR;
   }
   if (mp_set_double(&a, -0.0/0.0) != MP_VAL) {
      printf("\nmp_set_double should return MP_VAL for NaN");
      goto LBL_ERR;
   }

   for (i = 0; i < 1000; ++i) {
      int tmp = rand();
      double dbl = (double)tmp * rand() + 1;
      if (mp_set_double(&a, dbl) != MP_OKAY) {
         printf("\nmp_set_double() failed");
         goto LBL_ERR;
      }
      if (dbl != mp_get_double(&a)) {
         printf("\nmp_get_double() bad result!");
         goto LBL_ERR;
      }
      if (mp_set_double(&a, -dbl) != MP_OKAY) {
         printf("\nmp_set_double() failed");
         goto LBL_ERR;
      }
      if (-dbl != mp_get_double(&a)) {
         printf("\nmp_get_double() bad result!");
         goto LBL_ERR;
      }
   }
#endif

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;

}

static int test_mp_get_int(void)
{
   unsigned long t;
   int i;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < 1000; ++i) {
      t = (unsigned long)(rand() * rand() + 1) & 0xFFFFFFFFuL;
      mp_set_int(&a, t);
      if (t != mp_get_int(&a)) {
         printf("\nmp_get_int() bad result!");
         goto LBL_ERR;
      }
   }
   mp_set_int(&a, 0uL);
   if (mp_get_int(&a) != 0) {
      printf("\nmp_get_int() bad result!");
      goto LBL_ERR;
   }
   mp_set_int(&a, 0xFFFFFFFFuL);
   if (mp_get_int(&a) != 0xFFFFFFFFuL) {
      printf("\nmp_get_int() bad result!");
      goto LBL_ERR;
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_get_long(void)
{
   unsigned long s, t;
   int i;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < ((int)(sizeof(unsigned long)*CHAR_BIT) - 1); ++i) {
      t = (1ULL << (i+1)) - 1;
      if (!t)
         t = -1;
      printf(" t = 0x%lx i = %d\r", t, i);
      do {
         if (mp_set_long(&a, t) != MP_OKAY) {
            printf("\nmp_set_long() error!");
            goto LBL_ERR;
         }
         s = mp_get_long(&a);
         if (s != t) {
            printf("\nmp_get_long() bad result! 0x%lx != 0x%lx", s, t);
            goto LBL_ERR;
         }
         t <<= 1;
      } while (t != 0uL);
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_get_long_long(void)
{
   unsigned long long q, r;
   int i;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < ((int)(sizeof(unsigned long long)*CHAR_BIT) - 1); ++i) {
      r = (1ULL << (i+1)) - 1;
      if (!r)
         r = -1;
      printf(" r = 0x%llx i = %d\r", r, i);
      do {
         if (mp_set_long_long(&a, r) != MP_OKAY) {
            printf("\nmp_set_long_long() error!");
            goto LBL_ERR;
         }
         q = mp_get_long_long(&a);
         if (q != r) {
            printf("\nmp_get_long_long() bad result! 0x%llx != 0x%llx", q, r);
            goto LBL_ERR;
         }
         r <<= 1;
      } while (r != 0uLL);
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;

}

static int test_mp_sqrt(void)
{
   int i, n;

   mp_int a, b, c, d;
   if (mp_init_multi(&a, &b, &c, &d, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < 1000; ++i) {
      printf("%6d\r", i);
      fflush(stdout);
      n = (rand() & 15) + 1;
      mp_rand(&a, n);
      if (mp_sqrt(&a, &b) != MP_OKAY) {
         printf("\nmp_sqrt() error!");
         goto LBL_ERR;
      }
      mp_n_root_ex(&a, 2uL, &c, 0);
      mp_n_root_ex(&a, 2uL, &d, 1);
      if (mp_cmp_mag(&c, &d) != MP_EQ) {
         printf("\nmp_n_root_ex() bad result!");
         goto LBL_ERR;
      }
      if (mp_cmp_mag(&b, &c) != MP_EQ) {
         printf("mp_sqrt() bad result!\n");
         goto LBL_ERR;
      }
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int test_mp_is_square(void)
{
   int i, n;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (i = 0; i < 1000; ++i) {
      printf("%6d\r", i);
      fflush(stdout);

      /* test mp_is_square false negatives */
      n = (rand() & 7) + 1;
      mp_rand(&a, n);
      mp_sqr(&a, &a);
      if (mp_is_square(&a, &n) != MP_OKAY) {
         printf("\nfn:mp_is_square() error!");
         goto LBL_ERR;
      }
      if (n == 0) {
         printf("\nfn:mp_is_square() bad result!");
         goto LBL_ERR;
      }

      /* test for false positives */
      mp_add_d(&a, 1uL, &a);
      if (mp_is_square(&a, &n) != MP_OKAY) {
         printf("\nfp:mp_is_square() error!");
         goto LBL_ERR;
      }
      if (n == 1) {
         printf("\nfp:mp_is_square() bad result!");
         goto LBL_ERR;
      }

   }
   printf("\n\n");

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_sqrtmod_prime(void)
{
   struct mp_sqrtmod_prime_st {
      unsigned long p;
      unsigned long n;
      mp_digit r;
   };

   static struct mp_sqrtmod_prime_st sqrtmod_prime[] = {
      { 5, 14, 3 },
      { 7, 9, 4 },
      { 113, 2, 62 }
   };
   int i;

   mp_int a, b, c;
   if (mp_init_multi(&a, &b, &c, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* r^2 = n (mod p) */
   for (i = 0; i < (int)(sizeof(sqrtmod_prime)/sizeof(sqrtmod_prime[0])); ++i) {
      mp_set_int(&a, sqrtmod_prime[i].p);
      mp_set_int(&b, sqrtmod_prime[i].n);
      if (mp_sqrtmod_prime(&b, &a, &c) != MP_OKAY) {
         printf("Failed executing %d. mp_sqrtmod_prime\n", (i+1));
         goto LBL_ERR;
      }
      if (mp_cmp_d(&c, sqrtmod_prime[i].r) != MP_EQ) {
         printf("Failed %d. trivial mp_sqrtmod_prime\n", (i+1));
         ndraw(&c, "r");
         goto LBL_ERR;
      }
   }

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_expt_d(void)
{
   mp_int a, c;
   int e;
   mp_digit i, j;

   if ((e = mp_init_multi(&a, &c, NULL)) != MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (j = 3; j < 33; j+=2) {
      mp_set(&a, j);
#ifdef MP_8BIT
      for (i = 2; i < MP_MASK; i <<= 1) {
#elif (defined MP_16BIT)
      for (i = 2; i < MP_MASK; i <<= 1) {
#else
      for (i = 2; i < (1lu << 17); i <<= 1) {
#endif
         mp_expt_d(&a, i, &c);
         mp_n_root(&c, i, &c);
         if (mp_cmp(&c, &a) != MP_EQ) {
            fprintf(stderr,"mp_exp_d failed for %d^%d\n",(int) j,(int) i);
            goto LTM_ERR;
         }
      }

      mp_rand(&a, 10);
      for (i = 3; i < 10; i++) {
         mp_expt_d(&a, i, &c);
         mp_n_root(&c, i, &c);
         if (mp_cmp(&c, &a) != MP_EQ) {
            fprintf(stderr,"mp_exp_d failed for x^%d\n",(int) i);
            goto LTM_ERR;
         }
      }
   }
   mp_clear_multi(&a, &c, NULL);
   return EXIT_SUCCESS;
LTM_ERR:
   mp_clear_multi(&a, &c, NULL);
   return EXIT_FAILURE;
}

#if defined(LTM_DEMO_REAL_RAND) && !defined(_WIN32)
static FILE *fd_urandom = 0;
#endif

static int myrng(unsigned char *dst, int len, void *dat)
{
   int x;
   (void)dat;
#if defined(LTM_DEMO_REAL_RAND) && !defined(_WIN32)
   if (!fd_urandom) {
      fprintf(stderr, "\nno /dev/urandom\n");
   } else {
      return fread(dst, 1uL, len, fd_urandom);
   }
#endif
   for (x = 0; x < len;) {
      unsigned int r = (unsigned int)rand();
      do {
         dst[x++] = r & 0xFFu;
         r >>= 8;
      } while ((r != 0u) && (x < len));
   }
   return len;
}

static int test_mp_prime_random_ex(void)
{
   int ix, err;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* test for size */
   for (ix = 10; ix < 128; ix++) {
      printf("Testing (not safe-prime): %9d bits    \r", ix);
      fflush(stdout);
      err = mp_prime_random_ex(&a, 8, ix,
                               (rand() & 1) ? 0 : LTM_PRIME_2MSB_ON, myrng,
                               NULL);
      if (err != MP_OKAY) {
         printf("\nfailed with error: %s\n", mp_error_to_string(err));
         goto LBL_ERR;
      }
      if (mp_count_bits(&a) != ix) {
         printf("Prime is %d not %d bits!!!\n", mp_count_bits(&a), ix);
         goto LBL_ERR;
      }
   }
   printf("\n");

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_prime_is_prime(void)
{
   int ix, err, cnt;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* strong Miller-Rabin pseudoprime to the first 200 primes (F. Arnault) */
   puts("Testing mp_prime_is_prime() with Arnault's pseudoprime  803...901 \n");
   mp_read_radix(&a,
                 "91xLNF3roobhzgTzoFIG6P13ZqhOVYSN60Fa7Cj2jVR1g0k89zdahO9/kAiRprpfO1VAp1aBHucLFV/qLKLFb+zonV7R2Vxp1K13ClwUXStpV0oxTNQVjwybmFb5NBEHImZ6V7P6+udRJuH8VbMEnS0H8/pSqQrg82OoQQ2fPpAk6G1hkjqoCv5s/Yr",
                 64);
   mp_prime_is_prime(&a, 8, &cnt);
   if (cnt == MP_YES) {
      printf("Arnault's pseudoprime is not prime but mp_prime_is_prime says it is.\n");
      goto LBL_ERR;
   }
   /* About the same size as Arnault's pseudoprime */
   puts("Testing mp_prime_is_prime() with certified prime 2^1119 + 53\n");
   mp_set(&a, 1uL);
   mp_mul_2d(&a,1119,&a);
   mp_add_d(&a, 53uL, &a);
   err = mp_prime_is_prime(&a, 8, &cnt);
   /* small problem */
   if (err != MP_OKAY) {
      printf("\nfailed with error: %s\n", mp_error_to_string(err));
   }
   /* large problem */
   if (cnt == MP_NO) {
      printf("A certified prime is a prime but mp_prime_is_prime says it is not.\n");
   }
   if ((err != MP_OKAY) || (cnt == MP_NO)) {
      printf("prime tested was: ");
      mp_fwrite(&a,16,stdout);
      putchar('\n');
      goto LBL_ERR;
   }
   for (ix = 16; ix < 128; ix++) {
      printf("Testing (    safe-prime): %9d bits    \r", ix);
      fflush(stdout);
      err = mp_prime_random_ex(
               &a, 8, ix, ((rand() & 1) ? 0 : LTM_PRIME_2MSB_ON) | LTM_PRIME_SAFE,
               myrng, NULL);
      if (err != MP_OKAY) {
         printf("\nfailed with error: %s\n", mp_error_to_string(err));
         goto LBL_ERR;
      }
      if (mp_count_bits(&a) != ix) {
         printf("Prime is %d not %d bits!!!\n", mp_count_bits(&a), ix);
         goto LBL_ERR;
      }
      /* let's see if it's really a safe prime */
      mp_sub_d(&a, 1uL, &b);
      mp_div_2(&b, &b);
      err = mp_prime_is_prime(&b, 8, &cnt);
      /* small problem */
      if (err != MP_OKAY) {
         printf("\nfailed with error: %s\n", mp_error_to_string(err));
      }
      /* large problem */
      if (cnt == MP_NO) {
         printf("\nsub is not prime!\n");
      }
      if ((err != MP_OKAY) || (cnt == MP_NO)) {
         printf("prime tested was: ");
         mp_fwrite(&a,16,stdout);
         putchar('\n');
         printf("sub tested was: ");
         mp_fwrite(&b,16,stdout);
         putchar('\n');
         goto LBL_ERR;
      }

   }
   /* Check regarding problem #143 */
#ifndef MP_8BIT
   mp_read_radix(&a,
                 "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9A63A3620FFFFFFFFFFFFFFFF",
                 16);
   err = mp_prime_strong_lucas_selfridge(&a, &cnt);
   /* small problem */
   if (err != MP_OKAY) {
      printf("\nmp_prime_strong_lucas_selfridge failed with error: %s\n", mp_error_to_string(err));
   }
   /* large problem */
   if (cnt == MP_NO) {
      printf("\n\nissue #143 - mp_prime_strong_lucas_selfridge FAILED!\n");
   }
   if ((err != MP_OKAY) || (cnt == MP_NO)) {
      printf("prime tested was: ");
      mp_fwrite(&a,16,stdout);
      putchar('\n');
      goto LBL_ERR;
   }
#endif

   printf("\n\n");

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;

}

static int test_mp_montgomery_reduce(void)
{
   mp_digit mp;
   int ix, i, n;
   char buf[4096];

   mp_int a, b, c, d, e;
   if (mp_init_multi(&a, &b, &c, &d, &e, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* test montgomery */
   for (i = 1; i <= 10; i++) {
      if (i == 10)
         i = 1000;
      printf(" digit size: %2d\r", i);
      fflush(stdout);
      for (n = 0; n < 1000; n++) {
         mp_rand(&a, i);
         a.dp[0] |= 1;

         /* let's see if R is right */
         mp_montgomery_calc_normalization(&b, &a);
         mp_montgomery_setup(&a, &mp);

         /* now test a random reduction */
         for (ix = 0; ix < 100; ix++) {
            mp_rand(&c, 1 + abs(rand()) % (2*i));
            mp_copy(&c, &d);
            mp_copy(&c, &e);

            mp_mod(&d, &a, &d);
            mp_montgomery_reduce(&c, &a, mp);
            mp_mulmod(&c, &b, &a, &c);

            if (mp_cmp(&c, &d) != MP_EQ) {
/* *INDENT-OFF* */
               printf("d = e mod a, c = e MOD a\n");
               mp_todecimal(&a, buf); printf("a = %s\n", buf);
               mp_todecimal(&e, buf); printf("e = %s\n", buf);
               mp_todecimal(&d, buf); printf("d = %s\n", buf);
               mp_todecimal(&c, buf); printf("c = %s\n", buf);
               printf("compare no compare!\n"); goto LBL_ERR;
/* *INDENT-ON* */
            }
            /* only one big montgomery reduction */
            if (i > 10) {
               n = 1000;
               ix = 100;
            }
         }
      }
   }

   printf("\n\n");

   mp_clear_multi(&a, &b, &c, &d, &e, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, &e, NULL);
   return EXIT_FAILURE;

}

static int test_mp_read_radix(void)
{
   char buf[4096];

   mp_int a;
   if (mp_init_multi(&a, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   mp_read_radix(&a, "123456", 10);
   mp_toradix_n(&a, buf, 10, 3);
   printf("a == %s\n", buf);
   mp_toradix_n(&a, buf, 10, 4);
   printf("a == %s\n", buf);
   mp_toradix_n(&a, buf, 10, 30);
   printf("a == %s\n", buf);

#if 0
   for (;;) {
      fgets(buf, sizeof(buf), stdin);
      mp_read_radix(&a, buf, 10);
      mp_prime_next_prime(&a, 5, 1);
      mp_toradix(&a, buf, 10);
      printf("%s, %lu\n", buf, a.dp[0] & 3);
   }
#endif

   mp_clear_multi(&a, NULL);
   return EXIT_SUCCESS;
}

static int test_mp_cnt_lsb(void)
{
   int ix;

   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   mp_set(&a, 1uL);
   for (ix = 0; ix < 1024; ix++) {
      if (mp_cnt_lsb(&a) != ix) {
         printf("Failed at %d, %d\n", ix, mp_cnt_lsb(&a));
         goto LBL_ERR;
      }
      mp_mul_2(&a, &a);
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;

}

static int test_mp_reduce_2k(void)
{
   int ix, cnt;

   mp_int a, b, c, d;
   if (mp_init_multi(&a, &b, &c, &d, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* test mp_reduce_2k */
   for (cnt = 3; cnt <= 128; ++cnt) {
      mp_digit tmp;

      mp_2expt(&a, cnt);
      mp_sub_d(&a, 2uL, &a);  /* a = 2**cnt - 2 */

      printf("\r %4d bits", cnt);
      printf("(%d)", mp_reduce_is_2k(&a));
      mp_reduce_2k_setup(&a, &tmp);
      printf("(%lu)", (unsigned long) tmp);
      for (ix = 0; ix < 1000; ix++) {
         if (!(ix & 127)) {
            printf(".");
            fflush(stdout);
         }
         mp_rand(&b, (cnt / DIGIT_BIT + 1) * 2);
         mp_copy(&c, &b);
         mp_mod(&c, &a, &c);
         mp_reduce_2k(&b, &a, 2uL);
         if (mp_cmp(&c, &b) != MP_EQ) {
            printf("FAILED\n");
            goto LBL_ERR;
         }
      }
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int test_mp_div_3(void)
{
   int cnt;

   mp_int a, b, c, d, e;
   if (mp_init_multi(&a, &b, &c, &d, &e, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* test mp_div_3  */
   mp_set(&d, 3uL);
   for (cnt = 0; cnt < 10000;) {
      mp_digit r2;

      if (!(++cnt & 127)) {
         printf("%9d\r", cnt);
         fflush(stdout);
      }
      mp_rand(&a, abs(rand()) % 128 + 1);
      mp_div(&a, &d, &b, &e);
      mp_div_3(&a, &c, &r2);

      if (mp_cmp(&b, &c) || mp_cmp_d(&e, r2)) {
         printf("\nmp_div_3 => Failure\n");
         goto LBL_ERR;
      }
   }
   printf("\nPassed div_3 testing");

   mp_clear_multi(&a, &b, &c, &d, &e, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, &e, NULL);
   return EXIT_FAILURE;
}

static int test_mp_dr_reduce(void)
{
   mp_digit mp;
   int cnt;
   unsigned rr;
   int ix;

   mp_int a, b, c;
   if (mp_init_multi(&a, &b, &c, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }

   /* test the DR reduction */
   for (cnt = 2; cnt < 32; cnt++) {
      printf("\r%d digit modulus", cnt);
      mp_grow(&a, cnt);
      mp_zero(&a);
      for (ix = 1; ix < cnt; ix++) {
         a.dp[ix] = MP_MASK;
      }
      a.used = cnt;
      a.dp[0] = 3;

      mp_rand(&b, cnt - 1);
      mp_copy(&b, &c);

      rr = 0;
      do {
         if (!(rr & 127)) {
            printf(".");
            fflush(stdout);
         }
         mp_sqr(&b, &b);
         mp_add_d(&b, 1uL, &b);
         mp_copy(&b, &c);

         mp_mod(&b, &a, &b);
         mp_dr_setup(&a, &mp);
         mp_dr_reduce(&c, &a, mp);

         if (mp_cmp(&b, &c) != MP_EQ) {
            printf("Failed on trial %u\n", rr);
            goto LBL_ERR;
         }
      } while (++rr < 500);
      printf(" passed");
      fflush(stdout);
   }

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_reduce_2k_l(void)
{
#   if LTM_DEMO_TEST_REDUCE_2K_L
   mp_int a, b;
   if (mp_init_multi(&a, &b, NULL)!= MP_OKAY) {
      return EXIT_FAILURE;
   }
   /* test the mp_reduce_2k_l code */
#      if LTM_DEMO_TEST_REDUCE_2K_L == 1
   /* first load P with 2^1024 - 0x2A434 B9FDEC95 D8F9D550 FFFFFFFF FFFFFFFF */
   mp_2expt(&a, 1024);
   mp_read_radix(&b, "2A434B9FDEC95D8F9D550FFFFFFFFFFFFFFFF", 16);
   mp_sub(&a, &b, &a);
#      elif LTM_DEMO_TEST_REDUCE_2K_L == 2
   /*  p = 2^2048 - 0x1 00000000 00000000 00000000 00000000 4945DDBF 8EA2A91D 5776399B B83E188F  */
   mp_2expt(&a, 2048);
   mp_read_radix(&b,
                 "1000000000000000000000000000000004945DDBF8EA2A91D5776399BB83E188F",
                 16);
   mp_sub(&a, &b, &a);
#      else
#         error oops
#      endif

   mp_todecimal(&a, buf);
   printf("\n\np==%s\n", buf);
   /* now mp_reduce_is_2k_l() should return */
   if (mp_reduce_is_2k_l(&a) != 1) {
      printf("mp_reduce_is_2k_l() return 0, should be 1\n");
      goto LBL_ERR;
   }
   mp_reduce_2k_setup_l(&a, &d);
   /* now do a million square+1 to see if it varies */
   mp_rand(&b, 64);
   mp_mod(&b, &a, &b);
   mp_copy(&b, &c);
   printf("Testing: mp_reduce_2k_l...");
   fflush(stdout);
   for (cnt = 0; cnt < (int)(1UL << 20); cnt++) {
      mp_sqr(&b, &b);
      mp_add_d(&b, 1uL, &b);
      mp_reduce_2k_l(&b, &a, &d);
      mp_sqr(&c, &c);
      mp_add_d(&c, 1uL, &c);
      mp_mod(&c, &a, &c);
      if (mp_cmp(&b, &c) != MP_EQ) {
         printf("mp_reduce_2k_l() failed at step %d\n", cnt);
         mp_tohex(&b, buf);
         printf("b == %s\n", buf);
         mp_tohex(&c, buf);
         printf("c == %s\n", buf);
         goto LBL_ERR;
      }
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
#else
   return EXIT_SUCCESS;
#   endif /* LTM_DEMO_TEST_REDUCE_2K_L */
}


/*
   Cannot test mp_exp(_d) without mp_n_root and vice versa.
   So one of the two has to be tested from scratch.

   Numbers generated by
   for i in {1..10}
   do
     seed=$(head -c 10000 /dev/urandom | tr -dc '[:digit:]' | head -c 120);
     echo $seed;
     convertbase $seed 10  64;
   done

   (The program "convertbase" uses libtommath's to/from_radix functions)

   Roots were precalculated with Pari/GP

   default(realprecision,1000);
   for(n=3,100,r = floor(a^(1/n));printf("\"" r "\", "))

   All numbers as strings to simplifiy things, especially for the
   low-mp branch.
*/
static int test_mp_n_root(void)
{
   mp_int a, c, r;
   int e;
   int i, j;

   const char *input[] = {
      /*
       * 7695055368100873513971520489282333897557005032980617084180385070\
       * 39530690836861445204045548919181252491201760744537260787 x
       */
      "4n9cbk886QtLQmofprid3l2Q0GD8Yv979Lh8BdZkFE8g2pDUUSMBET/+M/YFyVZ3mBp",
      /*
       * 8668311813364777780629077601922380846241546283585346658481548505\
       * 15770928406118389455996364171707997206140804483448477830 x
       */
      "5NlgzHhmIX05O5YoW5yW5reAlVNtRAlIcN2dfoATnNdc1Cw5lHZUTwNthmK6/ZLKfY6",
      /*
       * 5923948132611980943162429619835240735669289172375390561536372873\
       * 00911012455497635806885159583259351684886304500667424473 x
       */
      "3gweiHDX+ji5utraSe46IJX+uuh7iggs63xIpMP5MriU4Np+LpHI5are8RzS9pKh9xP",
      /*
       * 8734756636775721743266740566772923450964420400602992970837352690\
       * 87632291744452124879574493682190594576826340595454525980 x
       */
      "5QOJUSKMrfe7LkeyJOlupS8h7bjT+TXmZkDzOjZtfj7mdA7cbg0lRX3CuafhjIrpK8S",
      /*
       * 6906200463716278995655031983489068277395617279812487007026442646\
       * 90715984250537069557543866104282110588763071389649149719 x
       */
      "4HtYFldVkyVbrlg/s7kmaA7j45PvLQm+1bbn6ehgP8tVoBmGbv2yDQI1iQQze4AlHyN",
      /*
       * 5797688045237028977800804043670890092752575196760186254305083786\
       * 22139392287523144766002998666506693902997364990227220905 x
       */
      "3bwCUx79NAR7c68OPSp5ZabhZ9aBEr7rWNTO2oMY7zhbbbw7p6shSMxqE9K9nrTNucf",
      /*
       * 7592540396118752335365242823215209154427610845641284467535865765\
       * 66144026031808712312179712267802078954238625080444510708 x
       */
      "4j5RGb78TfuYSzrXn0z6tiAoWiRI81hGY3el9AEa9S+gN4x/AmzotHT2Hvj6lyBpE7q",
      /*
       * 7663949070123305563643841881821433873426475515633527095211823358\
       * 92193882506834941622486768823922834949810956061095773326 x
       */
      "4lwg30SXqZhEHNsl5LIXdyu7UNt0VTWebP3m7+WUL+hsnFW9xJe7UnzYngZsvWh14IE",
      /*
       * 3199287858200533099895658842035281637583240610372074122891550803\
       * 86540175299848020023543385420537570948457000165872629485 x
       */
      "1+tcqFeRuGqjRADRoRUJ8gL4UUSFQVrVVoV6JpwVcKsuBq5G0pABn0dLcQQQMViiVRj",
      /*
       * 1097708713378916091335075933100856380360816322069710794134005170\
       * 92064545227259556173736894119720799430136890706570041659
       */
      "hXwxuFySNSFcmbrs/coz4FUAaUYaOEt+l4V5V8vY71KyBvQPxRq/6lsSrG2FHvWDax"
   };
   /* roots 3-100 of the above */
   const char *root[10][100] = {
      {
         "9163694094944489658600517465135586130944",
         "936597377180979771960755204040", "948947857956884030956907",
         "95727185767390496595", "133844854039712620", "967779611885360",
         "20926191452627", "974139547476", "79203891950", "9784027073",
         "1667309744", "365848129", "98268452", "31109156", "11275351",
         "4574515", "2040800", "986985", "511525", "281431", "163096",
         "98914", "62437", "40832", "27556", "19127", "13614", "9913",
         "7367", "5577", "4294", "3357", "2662", "2138", "1738", "1428",
         "1185", "993", "839", "715", "613", "530", "461", "403", "355",
         "314", "279", "249", "224", "202", "182", "166", "151", "138",
         "126", "116", "107", "99", "92", "85", "79", "74", "69", "65", "61",
         "57", "54", "51", "48", "46", "43", "41", "39", "37", "36", "34",
         "32", "31", "30", "28", "27", "26", "25", "24", "23", "23", "22",
         "21", "20", "20", "19", "18", "18", "17", "17", "16", "16", "15"
      }, {
         "9534798256755061606359588498764080011382",
         "964902943621813525741417593772", "971822399862464674540423",
         "97646291566833512831", "136141536090599560", "982294733581430",
         "21204945933335", "985810529393", "80066084985", "9881613813",
         "1682654547", "368973625", "99051783", "31341581", "11354620",
         "4604882", "2053633", "992879", "514434", "282959", "163942",
         "99406", "62736", "41020", "27678", "19208", "13670", "9952",
         "7395", "5598", "4310", "3369", "2671", "2145", "1744", "1433",
         "1189", "996", "842", "717", "615", "531", "462", "404", "356",
         "315", "280", "250", "224", "202", "183", "166", "151", "138",
         "127", "116", "107", "99", "92", "85", "80", "74", "70", "65", "61",
         "58", "54", "51", "48", "46", "43", "41", "39", "37", "36", "34",
         "32", "31", "30", "29", "27", "26", "25", "24", "23", "23", "22",
         "21", "20", "20", "19", "18", "18", "17", "17", "16", "16", "15"
      }, {
         "8398539113202579297642815367509019445624",
         "877309458945432597462853440936", "900579899458998599215071",
         "91643543761699761637", "128935656335800903", "936647990947203",
         "20326748623514", "948988882684", "77342677787", "9573063447",
         "1634096832", "359076114", "96569670", "30604705", "11103188",
         "4508519", "2012897", "974160", "505193", "278105", "161251",
         "97842", "61788", "40423", "27291", "18949", "13492", "9826",
         "7305", "5532", "4260", "3332", "2642", "2123", "1726", "1418",
         "1177", "986", "834", "710", "610", "527", "458", "401", "353",
         "312", "278", "248", "223", "201", "181", "165", "150", "137",
         "126", "116", "107", "99", "91", "85", "79", "74", "69", "65", "61",
         "57", "54", "51", "48", "46", "43", "41", "39", "37", "35", "34",
         "32", "31", "30", "28", "27", "26", "25", "24", "23", "22", "22",
         "21", "20", "20", "19", "18", "18", "17", "17", "16", "16", "15"
      }, {
         "9559098494021810340217797724866627755195",
         "966746709063325235560830083787", "973307706084821682248292",
         "97770642291138756434", "136290128605981259", "983232784778520",
         "21222944848922", "986563584410", "80121684894", "9887903837",
         "1683643206", "369174929", "99102220", "31356542", "11359721",
         "4606836", "2054458", "993259", "514621", "283057", "163997",
         "99437", "62755", "41032", "27686", "19213", "13674", "9955",
         "7397", "5599", "4311", "3370", "2672", "2146", "1744", "1433",
         "1189", "996", "842", "717", "615", "532", "462", "404", "356",
         "315", "280", "250", "224", "202", "183", "166", "151", "138",
         "127", "116", "107", "99", "92", "86", "80", "74", "70", "65", "61",
         "58", "54", "51", "48", "46", "43", "41", "39", "37", "36", "34",
         "32", "31", "30", "29", "27", "26", "25", "24", "23", "23", "22",
         "21", "20", "20", "19", "18", "18", "17", "17", "16", "16", "15"
      }, {
         "8839202025813295923132694443541993309220",
         "911611499784863252820288596270", "928640961450376817534853",
         "94017030509441723821", "131792686685970629", "954783483196511",
         "20676214073400", "963660189823", "78428929840", "9696237956",
         "1653495486", "363032624", "97562430", "30899570", "11203842",
         "4547110", "2029216", "981661", "508897", "280051", "162331",
         "98469", "62168", "40663", "27446", "19053", "13563", "9877",
         "7341", "5558", "4280", "3347", "2654", "2132", "1733", "1424",
         "1182", "990", "837", "713", "612", "529", "460", "402", "354",
         "313", "279", "249", "223", "201", "182", "165", "150", "138",
         "126", "116", "107", "99", "92", "85", "79", "74", "69", "65", "61",
         "57", "54", "51", "48", "46", "43", "41", "39", "37", "36", "34",
         "32", "31", "30", "28", "27", "26", "25", "24", "23", "23", "22",
         "21", "20", "20", "19", "18", "18", "17", "17", "16", "16", "15"
      }, {
         "8338442683973420410660145045849076963795",
         "872596990706967613912664152945", "896707843885562730147307",
         "91315073695274540969", "128539440806486007", "934129001105825",
         "20278149285734", "946946589774", "77191347471", "9555892093",
         "1631391010", "358523975", "96431070", "30563524", "11089126",
         "4503126", "2010616", "973111", "504675", "277833", "161100",
         "97754", "61734", "40390", "27269", "18934", "13482", "9819",
         "7300", "5528", "4257", "3330", "2641", "2122", "1725", "1417",
         "1177", "986", "833", "710", "609", "527", "458", "401", "353",
         "312", "278", "248", "222", "200", "181", "165", "150", "137",
         "126", "116", "107", "99", "91", "85", "79", "74", "69", "65", "61",
         "57", "54", "51", "48", "46", "43", "41", "39", "37", "35", "34",
         "32", "31", "30", "28", "27", "26", "25", "24", "23", "22", "22",
         "21", "20", "20", "19", "18", "18", "17", "17", "16", "16", "15"
      }, {
         "9122818552483814953977703257848970704164",
         "933462289569511464780529972314", "946405863353935713909178",
         "95513446972056321834", "133588658082928446",
         "966158521967027", "20895030642048", "972833934108",
         "79107381638", "9773098125", "1665590516", "365497822",
         "98180628", "31083090", "11266459", "4571108", "2039360",
         "986323", "511198", "281260", "163001", "98858",
         "62404", "40811", "27543", "19117", "13608", "9908",
         "7363", "5575", "4292", "3356", "2661", "2138",
         "1737", "1428", "1185", "993", "839", "714", "613",
         "530", "461", "403", "355", "314", "279", "249",
         "224", "202", "182", "165", "151", "138", "126",
         "116", "107", "99", "92", "85", "79", "74", "69",
         "65", "61", "57", "54", "51", "48", "46", "43",
         "41", "39", "37", "36", "34", "32", "31", "30",
         "28", "27", "26", "25", "24", "23", "23", "22",
         "21", "20", "20", "19", "18", "18", "17", "17",
         "16", "16", "15"
      }, {
         "9151329724083804100369546479681933027521",
         "935649419557299174433860420387", "948179413831316112751907",
         "95662582675170358900", "133767426788182384",
         "967289728859610", "20916775466497", "973745045600",
         "79174731802", "9780725058", "1666790321", "365742295",
         "98241919", "31101281", "11272665", "4573486", "2040365",
         "986785", "511426", "281380", "163067", "98897",
         "62427", "40826", "27552", "19124", "13612", "9911",
         "7366", "5576", "4294", "3357", "2662", "2138",
         "1738", "1428", "1185", "993", "839", "715", "613",
         "530", "461", "403", "355", "314", "279", "249",
         "224", "202", "182", "165", "151", "138", "126",
         "116", "107", "99", "92", "85", "79", "74", "69",
         "65", "61", "57", "54", "51", "48", "46", "43",
         "41", "39", "37", "36", "34", "32", "31", "30",
         "28", "27", "26", "25", "24", "23", "23", "22",
         "21", "20", "20", "19", "18", "18", "17", "17",
         "16", "16", "15"
      }, {
         "6839396355168045468586008471269923213531",
         "752078770083218822016981965090", "796178899357307807726034",
         "82700643015444840424", "118072966296549115",
         "867224751770392", "18981881485802", "892288574037",
         "73130030771", "9093989389", "1558462688", "343617470",
         "92683740", "29448679", "10708016", "4356820", "1948676",
         "944610", "490587", "270425", "156989", "95362",
         "60284", "39477", "26675", "18536", "13208", "9627",
         "7161", "5426", "4181", "3272", "2596", "2087",
         "1697", "1395", "1159", "971", "821", "700", "601",
         "520", "452", "396", "348", "308", "274", "245",
         "220", "198", "179", "163", "148", "136", "124",
         "114", "106", "98", "91", "84", "78", "73", "68",
         "64", "60", "57", "53", "50", "48", "45", "43",
         "41", "39", "37", "35", "34", "32", "31", "29",
         "28", "27", "26", "25", "24", "23", "22", "22",
         "21", "20", "19", "19", "18", "18", "17", "17",
         "16", "16", "15"
      }, {
         "4788090721380022347683138981782307670424",
         "575601315594614059890185238256", "642831903229558719812840",
         "69196031110028430211", "101340693763170691",
         "758683936560287", "16854690815260", "801767985909",
         "66353290503", "8318415180", "1435359033", "318340531",
         "86304307", "27544217", "10054988", "4105446", "1841996",
         "895414", "466223", "257591", "149855", "91205",
         "57758", "37886", "25639", "17842", "12730", "9290",
         "6918", "5248", "4048", "3170", "2518", "2026",
         "1649", "1357", "1128", "946", "800", "682", "586",
         "507", "441", "387", "341", "302", "268", "240",
         "215", "194", "176", "160", "146", "133", "122",
         "112", "104", "96", "89", "83", "77", "72", "67",
         "63", "59", "56", "53", "50", "47", "45", "42",
         "40", "38", "36", "35", "33", "32", "30", "29",
         "28", "27", "26", "25", "24", "23", "22", "21",
         "21", "20", "19", "19", "18", "17", "17", "16",
         "16", "15", "15"
      }
   };

   if ((e = mp_init_multi(&a, &c, &r, NULL)) != MP_OKAY) {
      return EXIT_FAILURE;
   }
#ifdef MP_8BIT
   for (i = 0; i < 1; i++) {
#else
   for (i = 0; i < 10; i++) {
#endif
      mp_read_radix(&a, input[i], 64);
#ifdef MP_8BIT
      for (j = 3; j < 10; j++) {
#else
      for (j = 3; j < 100; j++) {
#endif
         mp_n_root(&a, (mp_digit) j, &c);
         mp_read_radix(&r, root[i][j-3], 10);
         if (mp_cmp(&r, &c) != MP_EQ) {
            fprintf(stderr, "mp_n_root failed at input #%d, root #%d\n", i, j);
            goto LTM_ERR;
         }
      }
   }
   mp_clear_multi(&a, &c, &r, NULL);
   return EXIT_SUCCESS;
LTM_ERR:
   mp_clear_multi(&a, &c, &r, NULL);
   return EXIT_FAILURE;
}


#ifdef LTM_USE_EXTRA_FUNCTIONS
#if ( (defined LTM_USE_FASTER_VERSIONS) || (defined LTM_USE_FASTER_RADIX_SIZE) )


/* stripped down version of mp_radix_size. The faster version can be off by up to +3  */
static int s_rs(const mp_int *a, int radix, int *size)
{
   int     res, digs = 0;
   mp_int  t;
   mp_digit d;
   *size = 0;
   if (mp_iszero(a) == MP_YES) {
      *size = 2;
      return MP_OKAY;
   }
   if (radix == 2) {
      *size = mp_count_bits(a) + 1;
      return MP_OKAY;
   }
   if ((res = mp_init_copy(&t, a)) != MP_OKAY) {
      return res;
   }
   t.sign = MP_ZPOS;
   while (mp_iszero(&t) == MP_NO) {
      if ((res = mp_div_d(&t, (mp_digit)radix, &t, &d)) != MP_OKAY) {
         mp_clear(&t);
         return res;
      }
      ++digs;
   }
   mp_clear(&t);
   *size = digs + 1;
   return MP_OKAY;
}
#endif
static int test_mp_expt(void)
{
   mp_int a, b, c;
   int e;
   mp_digit i, j;

   if ((e = mp_init_multi(&a, &b, &c, NULL)) != MP_OKAY) {
      return EXIT_FAILURE;
   }

   for (j = 3; j < 33; j+=2) {
      mp_set(&a, j);
#ifdef MP_8BIT
      for (i = 2; i < MP_MASK; i <<= 1) {
#elif (defined MP_16BIT)
      for (i = 2; i < MP_MASK; i <<= 1) {
#else
      for (i = 2; i < (1lu << 17); i <<= 1) {
#endif
         mp_set(&b, i);
         mp_expt(&a, &b, &c);
         mp_n_root(&c, b.dp[0], &c);
         if (mp_cmp(&c, &a) != MP_EQ) {
            fprintf(stderr,"mp_exp failed for %d^%d\n", (int) j, (int) i);
            goto LTM_ERR;
         }
      }

      mp_rand(&a, 10);
      for (i = 3; i < 10; i++) {
         mp_set(&b, i);
         mp_expt(&a, &b, &c);
         mp_n_root(&c, b.dp[0], &c);
         if (mp_cmp(&c, &a) != MP_EQ) {
            fprintf(stderr,"mp_exp failed for x^%d\n", (int) i);
            goto LTM_ERR;
         }
      }
   }
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LTM_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_ilogb(void)
{
   mp_int a, lb;
   mp_digit d, base;
   int size;

   mp_init_multi(&a, &lb, NULL);

   /*
     base   a    result
      0     x    MP_VAL
      1     x    MP_VAL
   */
   mp_set(&a, 42uL);
   base = 0uL;
   if (mp_ilogb(&a, base, &lb) != MP_VAL) {
      goto LBL_ERR;
   }
   base = 1uL;
   if (mp_ilogb(&a, base, &lb) != MP_VAL) {
      goto LBL_ERR;
   }
   /*
     base   a    result
      2     0    MP_VAL
      2     1    0
      2     2    1
      2     3    1
   */
   base = 2uL;
   mp_zero(&a);
   if (mp_ilogb(&a, base, &lb) != MP_VAL) {
      goto LBL_ERR;
   }

   for (d = 1; d < 4; d++) {
      mp_set(&a, d);
      if (mp_ilogb(&a, base, &lb) != MP_OKAY) {
         goto LBL_ERR;
      }
      if (mp_cmp_d(&lb, (d == 1)?0uL:1uL) != MP_EQ) {
         goto LBL_ERR;
      }
   }
   /*
    base   a    result
     3     0    MP_VAL
     3     1    0
     3     2    0
     3     3    1
   */
   base = 3uL;
   mp_zero(&a);
   if (mp_ilogb(&a, base, &lb) != MP_VAL) {
      goto LBL_ERR;
   }
   for (d = 1; d < 4; d++) {
      mp_set(&a, d);
      if (mp_ilogb(&a, base, &lb) != MP_OKAY) {
         goto LBL_ERR;
      }
      if (mp_cmp_d(&lb, (d < base)?0uL:1uL) != MP_EQ) {
         goto LBL_ERR;
      }
   }

   /*
     bases 2..64 with "a" a random large constant.
     The range of bases tested allows to check with
     radix_size.
   */
   mp_rand(&a, 10);
   for (base = 2uL; base < 65uL; base++) {
      if (mp_ilogb(&a, base, &lb) != MP_OKAY) {
         goto LBL_ERR;
      }
#if ( (defined LTM_USE_FASTER_VERSIONS) || (defined LTM_USE_FASTER_RADIX_SIZE) )
      if (s_rs(&a,(int)base, &size) != MP_OKAY) {
         goto LBL_ERR;
      }
#else
      if (mp_radix_size(&a,(int)base, &size) != MP_OKAY) {
         goto LBL_ERR;
      }
#endif
      /* radix_size includes the memory needed for '\0', too*/
      size -= 2;
      if (mp_cmp_d(&lb, size) != MP_EQ) {
         goto LBL_ERR;
      }
   }

   /*Test upper edgecase with base MP_MASK and number (MP_MASK/2)*MP_MASK^10  */
   mp_set(&a, MP_MASK);
   if (mp_expt_d(&a, 10uL, &a) != MP_OKAY) {
      goto LBL_ERR;
   }
   if (mp_add_d(&a, (MP_MASK>>1), &a) != MP_OKAY) {
      goto LBL_ERR;
   }
   if (mp_ilogb(&a, MP_MASK, &lb) != MP_OKAY) {
      goto LBL_ERR;
   }
   if (mp_cmp_d(&lb, 10uL) != MP_EQ) {
      goto LBL_ERR;
   }

   mp_clear_multi(&a, &lb, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &lb, NULL);
   return EXIT_FAILURE;
}

static int test_mp_is_small_prime(void)
{
   mp_sieve *base = NULL;
   mp_sieve *segment = NULL;
   LTM_SIEVE_UINT single_segment_a = 0;
   int e;
   int i, test_size;

   LTM_SIEVE_UINT to_test[] = {
      52, 137, 153, 179, 6, 153, 53, 132, 150, 65,
      27414, 36339, 36155, 11067, 52060, 5741,
      29755, 2698, 52572, 13053, 9375, 47241
#ifndef MP_8BIT
      ,39626, 207423, 128857, 37419, 141696, 189465,
      41503, 127370, 91673, 8473, 479142414, 465566339,
      961126169, 1057886067, 1222702060, 1017450741,
      1019879755, 72282698, 2048787577, 2058368053
#endif
   };
   LTM_SIEVE_UINT tested[] = {
      0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0
#ifndef MP_8BIT
      ,0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0
#endif
   };
   LTM_SIEVE_UINT result;
   /*
      if ((e = mp_sieve_init(base)) != MP_OKAY) {
         fprintf(stderr,"mp_sieve_init(base) failed: \"%s\"\n",mp_error_to_string(e));
         exit(EXIT_FAILURE);
      }

      if ((e = mp_sieve_init(segment)) != MP_OKAY) {
         fprintf(stderr,"mp_sieve_init(segment) failed: \"%s\"\n",mp_error_to_string(e));
         goto LTM_ERR;
      }
   */
   test_size = (int)(sizeof(to_test)/sizeof(LTM_SIEVE_UINT));

   for (i = 0; i < test_size; i++) {
      if ((e = mp_is_small_prime(to_test[i], &result, &base, &segment, &single_segment_a)) != MP_OKAY) {
         fprintf(stderr,"mp_is_small_prime failed: \"%s\"\n",mp_error_to_string(e));
         goto LTM_ERR;
      }
      if (result != tested[i]) {
         fprintf(stderr,"mp_is_small_prime failed for %u. Said %u but is %u \n",
                 (unsigned int)to_test[i], (unsigned int)result, (unsigned int)tested[i]);
         goto LTM_ERR;
      }
   }

   mp_sieve_clear(base);
   mp_sieve_clear(segment);
   return EXIT_SUCCESS;
LTM_ERR:
   mp_sieve_clear(base);
   mp_sieve_clear(segment);
   return EXIT_FAILURE;
}
static int test_mp_next_small_prime(void)
{
   mp_sieve *base = NULL;
   mp_sieve *segment = NULL;
   LTM_SIEVE_UINT single_segment_a = 0;
   LTM_SIEVE_UINT ret;
   int e;
   int i, test_size;

   LTM_SIEVE_UINT to_test[] = {
      52, 137, 153, 179, 6, 153, 53, 132, 150, 65,
      27414, 36339, 36155, 11067, 52060, 5741,
      29755, 2698, 52572, 13053, 9375, 47241
#ifndef MP_8BIT
      ,39626, 207423, 128857, 37419, 141696, 189465,
      41503, 127370, 91673, 8473, 479142414, 465566339,
      961126169, 1057886067, 1222702060, 1017450741,
      1019879755, 72282698, 2048787577, 2058368053
#endif
   };
   LTM_SIEVE_UINT tested[] = {
      53, 137, 157, 179, 7, 157, 53, 137, 151, 67,
      27427, 36341, 36161, 11069, 52067, 5741,
      29759, 2699, 52579, 13063, 9377, 47251
#ifndef MP_8BIT
      ,39631, 207433, 128857, 37423, 141697, 189467,
      41507, 127373, 91673, 8501, 479142427, 465566393,
      961126169, 1057886083, 1222702081, 1017450823,
      1019879761, 72282701, 2048787577, 2058368113
#endif
   };

   test_size = (int)(sizeof(to_test)/sizeof(LTM_SIEVE_UINT));

   for (i = 0; i < test_size; i++) {
      if ((e = mp_next_small_prime(to_test[i], &ret, &base,
                                   &segment, &single_segment_a)) != MP_OKAY) {
         fprintf(stderr,"mp_next_small_prime failed with \"%s\" at index %d\n",
                 mp_error_to_string(e), i);
         goto LTM_ERR;
      }
      if (ret != tested[i]) {
         fprintf(stderr,"mp_next_small_prime failed for %u. Said %u but is %u \n",
                 (unsigned int)to_test[i], (unsigned int)ret, (unsigned int)tested[i]);
         goto LTM_ERR;
      }
   }

   mp_sieve_clear(base);
   mp_sieve_clear(segment);
   return EXIT_SUCCESS;
LTM_ERR:
   mp_sieve_clear(segment);
   mp_sieve_clear(base);
   return EXIT_FAILURE;
}

static int test_mp_prec_small_prime(void)
{
   mp_sieve *base = NULL;
   mp_sieve *segment = NULL;
   LTM_SIEVE_UINT single_segment_a = 0;
   LTM_SIEVE_UINT ret;
   int e;
   int i, test_size;

   LTM_SIEVE_UINT to_test[] = {
      52, 137, 153, 179, 6, 153, 53, 132, 150, 65,
      27414, 36339, 36155, 11067, 52060, 5741,
      29755, 2698, 52572, 13053, 9375, 47241
#ifndef MP_8BIT
      ,39626, 207423, 128857, 37419, 141696, 189465,
      41503, 127370, 91673, 8473, 479142414, 465566339,
      961126169, 1057886067, 1222702060, 1017450741,
      1019879755, 72282698, 2048787577, 2058368053
#endif
   };
   LTM_SIEVE_UINT tested[] = {
      47, 137, 151, 179, 5, 151, 53, 131, 149, 61,
      27409, 36319, 36151, 11059, 52057, 5741,
      29753, 2693, 52571, 13049, 9371, 47237
#ifndef MP_8BIT
      ,39623, 207409, 128857, 37409, 141689, 189463,
      41491, 127363, 91673, 8467, 479142413, 465566323,
      961126169, 1057886029, 1222702051, 1017450739,
      1019879717, 72282697, 2048787577, 2058368051
#endif
   };
   test_size = (int)(sizeof(to_test)/sizeof(LTM_SIEVE_UINT));

   for (i = 0; i < test_size; i++) {
      if ((e = mp_prec_small_prime(to_test[i], &ret, &base,
                                   &segment, &single_segment_a)) != MP_OKAY) {
         fprintf(stderr,"mp_prec_small_prime failed with \"%s\" at index %d\n",
                 mp_error_to_string(e), i);
         goto LTM_ERR;
      }
      if (ret != tested[i]) {
         fprintf(stderr,"mp_prec_small_prime failed for %u. Said %u but is %u \n",
                 (unsigned int)to_test[i], (unsigned int)ret, (unsigned int)tested[i]);
         goto LTM_ERR;
      }
   }

   mp_sieve_clear(base);
   mp_sieve_clear(segment);
   return EXIT_SUCCESS;
LTM_ERR:
   mp_sieve_clear(segment);
   mp_sieve_clear(base);
   return EXIT_FAILURE;
}

static int test_mp_small_prime_array(void)
{
   mp_sieve *base = NULL;
   mp_sieve *segment = NULL;
   LTM_SIEVE_UINT single_segment_a = 0;
   LTM_SIEVE_UINT *prime_array = NULL, array_size, j;
   int e;
   int i, test_size;
   mp_int total, t;

   /* For simplicity: just check the primesums */
   LTM_SIEVE_UINT to_test_start[] = {
      124, 147, 185, 291, 311, 329
#ifndef MP_8BIT
      , 7901, 19489, 20524, 47371, 50741, 59233
#endif
   };
   LTM_SIEVE_UINT to_test_end[] = {
      479, 488, 661, 671, 703, 833
#ifndef MP_8BIT
      , 66339, 68053, 72241, 81055, 82698, 86067
#endif
   };
   LTM_SIEVE_UINT tested[] = {
      18466, 18419, 33441, 28906, 31731, 45475
#ifndef MP_8BIT
      , 203371931, 197821202, 221915352, 195494605, 192053247, 172585395
#endif
   };
   /* TODO: add a pair or two that overlap segments */
   if ((e = mp_init_multi(&total, &t, NULL)) != MP_OKAY) {
      fprintf(stderr, "mp_init_multi(segment): \"%s\"\n", mp_error_to_string(e));
      goto LTM_ERR_1;
   }

   test_size = (int)(sizeof(to_test_start) / sizeof(LTM_SIEVE_UINT));

   for (i = 0; i < test_size; i++) {
      mp_zero(&total);
      /* It is always a different piece of memory */
      free(prime_array);
      if ((e = mp_small_prime_array(to_test_start[i], to_test_end[i], &prime_array,
                                    &array_size, &base,
                                    &segment, &single_segment_a)) != MP_OKAY) {
         fprintf(stderr, "mp_small_prime_array failed with \"%s\" for %u %u\n",
                 mp_error_to_string(e), to_test_start[i], to_test_end[i]);
         goto LTM_ERR;
      }
      for (j = 0; j < array_size; j++) {
#ifdef MP_64BIT
         if ((e = mp_add_d(&total, (mp_digit)prime_array[j], &total)) != MP_OKAY) {
            fprintf(stderr,"mp_add_d failed: \"%s\"\n",mp_error_to_string(e));
            goto LTM_ERR;
         }
#else
         if ((e = mp_set_long(&t,(mp_word)prime_array[j])) != MP_OKAY) {
            fprintf(stderr,"mp_set_long (1) failed: \"%s\"\n",mp_error_to_string(e));
            goto LTM_ERR;
         }
         if ((e = mp_add(&total, &t, &total)) != MP_OKAY) {
            fprintf(stderr,"mp_add failed: \"%s\"\n",mp_error_to_string(e));
            goto LTM_ERR;
         }
#endif
      }
#ifdef MP_64BIT
      if (mp_cmp_d(&total, (mp_digit)tested[i]) != MP_EQ) {
         fprintf(stderr,
                 "mp_small_prime_array primesum failed for %u %u == %u not ",
                 to_test_start[i], to_test_end[i], tested[i]);
         goto LTM_ERR;
      }
#else
      if ((e = mp_set_long(&t,(mp_word)tested[i])) != MP_OKAY) {
         fprintf(stderr,"mp_set_long (1) failed: \"%s\"\n",mp_error_to_string(e));
         goto LTM_ERR;
      }
      if (mp_cmp(&total, &t) != MP_EQ) {
         fprintf(stderr,
                 "mp_small_prime_array (8-bit) primesum failed for %u %u == %u not ",
                 to_test_start[i], to_test_end[i], tested[i]);
         mp_fwrite(&total, 10,stdout);
         putchar('\n');
         goto LTM_ERR;
      }
#endif
   }

   mp_clear_multi(&total, &t, NULL);
   mp_sieve_clear(base);
   mp_sieve_clear(segment);
   free(prime_array);
   return EXIT_SUCCESS;
LTM_ERR:
   mp_clear_multi(&total, &t, NULL);
LTM_ERR_1:
   mp_sieve_clear(segment);
   mp_sieve_clear(base);
   free(prime_array);
   return EXIT_FAILURE;
}

static int test_mp_factor(void)
{
   mp_int z;
   int e, i, bsize;
   unsigned char *tmp, maskAND;
   mp_factors factors;
   const char *nonprimes[] = {
      /* two errors, three shortcuts and a square */
      "0", "-1", "1", "2", "3", "4",
      /* semiprimes */
      "312157", "648195487", "598356255307", "991871964663707",
      /* with small and big factors */
      "991871964663708", "78039283755043329410", "325871418105900",
      /* with a lot of small primes */
      "14729944978144", "8080702298549993133544921875",
      /* 131101^11 , 131101 = 2^17 + 29, normal base sieve size is 2^16 */
      "196637365999125071351142476168035772619202756451412992101",
      /* with several medium primes */
      "133084923282398316896573492566258463894017",
      "4976502333248527146593643321629612436176722881994033642561942342220395609264488653641519",
      /* perfect powers */
      "116666335940207089610013731646382987819638245180475051703891993791182706173489874198001",
      "3235788747338312671076928222346879975613932865492908135711553936877133193671329830278801"
   };

   mp_init(&z);
   mp_factors_init(&factors);
   mp_zero(&z);

  puts("");

   for (i = 0; i < 2; i++) {
      if ((e = mp_factor(&z,&factors)) != MP_VAL) {
         goto LTM_ERR;
      }
   }

   for (i = 2; i < (int)(sizeof(nonprimes)/sizeof(nonprimes[0])); i++) {

      if ((e = mp_read_radix(&z, nonprimes[i], 10)) != MP_OKAY) {
         goto LTM_ERR;
      }

      if ((e = mp_factor(&z,&factors)) != MP_OKAY) {
         goto LTM_ERR;
      }
   }
   /* Random numbers of sizes up to and including 70 bits */
   for (i = 10; i <= 70; i+=5) {
      /* If that code looks familiar to you it is a c&p from mp_prime_random_ex */
      bsize = (i>>3) + ((i&7)?1:0);
      tmp = malloc((size_t)bsize);
      if (tmp == NULL) {
         fprintf(stderr,"malloc failed: \n");
         goto LTM_ERR;
      }

      if ((e = myrng(tmp, bsize, NULL)) != bsize) {
         fprintf(stderr,"mrng failed: %d\n",e);
         goto LTM_ERR;
      }

      maskAND = ((i&7) == 0) ? 0xFF : (0xFF >> (8 - (i & 7)));
      tmp[0] &= maskAND;
      tmp[0] |= 1 << ((i - 1) & 7);

      if ((e = mp_read_unsigned_bin(&z, tmp, bsize)) != MP_OKAY) {
         fprintf(stderr,"mp_read_unsigned_bin failed: \"%s\"\n",mp_error_to_string(e));
         goto LTM_ERR;
      }
      if ((e = mp_factor(&z,&factors)) != MP_OKAY) {
         goto LTM_ERR;
      }
      mp_fwrite(&z, 10, stdout);printf(" -> ");
      mp_factors_print(&factors, 10, 0, stdout); puts("");
      free(tmp);
   }
   /* The tests for perfect power had been switched off in mp_factor for now. */
   /*
       Stresstest for the trial division mp_trial.

       ~6.6429 E28304  A bit brutal, admitted.
       Too brutal for this algorithm. The perfect-power is the culprit.
       Useful for random numbers but definitely not for large primorials.
       About 48 seconds with the perfect-power tests and about 14 seconds without.
       And that with the optimized nth-root. With a normal nth-root it is
       already at ... uhm ... gave up waiting after 18 minutes.
       TODO: add a switch to mp_factor?
    */
    /*
   if ((e = mp_primorial((LTM_SIEVE_UINT)65535, &z)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_factor(&z,&factors)) != MP_OKAY) {
      goto LTM_ERR;
   }
   */
   mp_clear(&z);
   mp_factors_clear(&factors);
   return EXIT_SUCCESS;
LTM_ERR:
   mp_clear(&z);
   mp_factors_clear(&factors);
   return EXIT_FAILURE;
}

static int test_mp_prime_is_prime_deterministic(void){
   mp_int z;
   int e, i, result;
   LTM_SIEVE_UINT limit;
   const char *input_error[] = {
      "0", "-1"
   };
   const char *input_primes[] = {
      "839", "563", "887", "719", "587",
      "28607", "30467", "32507", "23627", "26927",
      "787187", "701783", "822167", "649787", "907967",
      "739824114959", "583566319823", "900970083227",
      "855726000172944154021782813479", "852922373435970513728235103247",
   };
   const char *input_composites[] = {
      /* https://oeis.org/A014233 */
      "2047", "1373653", "25326001", "3215031751", "2152302898747",
      "3474749660383", "341550071728321", "341550071728321",
      "3825123056546413051", "3825123056546413051", "3825123056546413051",
      "318665857834031151167461", "3317044064679887385961981"
   };

   mp_init(&z);

   /* Checking erroneous input, must fail */
   for(i = 0; i < 2; i++){
      if ((e = mp_read_radix(&z, input_error[i], 10)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if( (e = mp_prime_is_prime_deterministic(&z, &result) ) == MP_OKAY){
         goto LTM_ERR;
      }
   }

   /* Checking primes, must answer with MP_YES */
   for(i = 0; i < (sizeof(input_primes)/sizeof(input_primes[0])); i++){
      if ((e = mp_read_radix(&z, input_primes[i], 10)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if( (e = mp_prime_is_prime_deterministic(&z, &result) ) != MP_OKAY){
         goto LTM_ERR;
      }
      if(result != MP_YES) {
         goto LTM_ERR;
      }
   }

   /* Checking composites, must answer with MP_NO */
   for(i = 0; i < (sizeof(input_composites)/sizeof(input_composites[0])); i++){
      if ((e = mp_read_radix(&z, input_composites[i], 10)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if( (e = mp_prime_is_prime_deterministic(&z, &result) ) != MP_OKAY){
         goto LTM_ERR;
      }
      if(result != MP_NO) {
         goto LTM_ERR;
      }
   }

   mp_clear(&z);
   return EXIT_SUCCESS;
LTM_ERR:
   mp_clear(&z);
   return EXIT_FAILURE;
}

#endif /* LTM_USE_EXTRA_FUNCTIONS */

int unit_tests(void)
{
   static const struct {
      const char *name;
      int (*fn)(void);
   } test[] = {
#define T(n) { #n, test_##n }
          T(trivial_stuff),
          T(mp_cnt_lsb),
          T(mp_complement),
          T(mp_div_3),
          T(mp_dr_reduce),
          T(mp_n_root),
          T(mp_get_int),
          T(mp_get_long),
          T(mp_get_long_long),
          T(mp_invmod),
          T(mp_is_square),
          T(mp_jacobi),
          T(mp_kronecker),
          T(mp_montgomery_reduce),
          T(mp_prime_is_prime),
          T(mp_prime_random_ex),
          T(mp_read_radix),
          T(mp_reduce_2k),
          T(mp_reduce_2k_l),
          T(mp_set_double),
          T(mp_sqrt),
          T(mp_sqrtmod_prime),
          T(mp_expt_d),
          T(mp_tc_and),
          T(mp_tc_div_2d),
          T(mp_tc_or),
          T(mp_tc_xor)
#ifdef LTM_USE_EXTRA_FUNCTIONS
         ,T(mp_expt),
          T(mp_ilogb),
          T(mp_is_small_prime),
          T(mp_next_small_prime),
          T(mp_prec_small_prime),
          T(mp_small_prime_array),
          T(mp_factor),
          T(mp_prime_is_prime_deterministic)
#endif
#undef T
   };
   unsigned long i;
   int res = EXIT_SUCCESS;

#if defined(LTM_DEMO_REAL_RAND) && !defined(_WIN32)
   fd_urandom = fopen("/dev/urandom", "r");
   if (!fd_urandom) {
      fprintf(stderr, "\ncould not open /dev/urandom\n");
   }
#endif

   for (i = 0; i < sizeof(test) / sizeof(test[0]); ++i) {
      printf("TEST %s ", test[i].name);
      if (test[i].fn() != EXIT_SUCCESS) {
         puts("FAIL");
         res = EXIT_FAILURE;
         break;
      }
      puts("PASS");
   }

#if defined(LTM_DEMO_REAL_RAND) && !defined(_WIN32)
   if (fd_urandom) {
      fclose(fd_urandom);
   }
#endif
   return res;
}

