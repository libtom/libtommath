#include "shared.h"
#if ((defined __m68k__) || (defined __MC68K__) || (defined M68000))
/* VERY simpel comparing function, for use in this case and this case only! */
/* There is such a macro in tommath_private.h which is not include'd in this listing */
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
static double s_abs(double s_d)
{
   return (s_d < 0.0)?-s_d:s_d;
}
static float s_absf(float s_d)
{
   return (s_d < 0.0f)?-s_d:s_d;
}
#include <float.h>
static int s_compare_doubles(double s_a, double s_b)
{
   double abs_a, abs_b, delta;

   /* NaN, inf's and subnormals ignored, not needed in this case*/

   if (s_a == s_b) {
      return 1;
   }
#ifdef LTM_WARN_X87_EXT_PREC
   fprintf(stderr, "Warning: extended precision detected for 'double# test %g != %g, using fallback test\n", s_a, s_b);
#endif
   abs_a = s_abs(s_a);
   abs_b = s_abs(s_b);
   delta = s_abs(s_a - s_b);

   return ((delta/MIN(abs_a, abs_b)) <  DBL_EPSILON);
}

static int s_compare_floats(float s_a, float s_b)
{
   double abs_a, abs_b, delta;

   /* NaN, inf's and subnormals ignored, not needed in this case*/

   if (s_a == s_b) {
      return 1;
   }
#ifdef LTM_WARN_X87_EXT_PREC
   fprintf(stderr, "Warning: extended precision detected for 'float' test %g != %g, using fallback test\n", s_a, s_b);
#endif
   abs_a = s_absf(s_a);
   abs_b = s_absf(s_b);
   delta = s_absf(s_a - s_b);

   return ((delta/MIN(abs_a, abs_b)) <  FLT_EPSILON);
}

static int s_compare_long_double(long double s_a, long double s_b)
{
   double abs_a, abs_b, delta;

   /* NaN, inf's and subnormals ignored, not needed in this case*/

   if (s_a == s_b) {
      return 1;
   }
#ifdef LTM_WARN_X87_EXT_PREC
   fprintf(stderr, "Warning: extended precision detected for 'long double' test  %Lg != %Lg, using fallback test \n", s_a,
           s_b);
#endif
   abs_a = s_absf(s_a);
   abs_b = s_absf(s_b);
   delta = s_absf(s_a - s_b);

   return ((delta/MIN(abs_a, abs_b)) <  LDBL_EPSILON);
}
#define S_COMPARE_DOUBLE(x,y) s_compare_doubles((x),(y))
#define S_COMPARE_FLOAT(x,y) s_compare_floats((x),(y))
#define S_COMPARE_LONG_DOUBLE(x,y) s_compare_long_double((x),(y))
#else
#define S_COMPARE_FLOAT(x,y) ( (x) == (y) )
#define S_COMPARE_DOUBLE(x,y) ( (x) == (y) )
#define S_COMPARE_LONG_DOUBLE(x,y) ( (x) == (y) )
#endif

float flt_count;
double dbl_count;
#include <float.h>
#if ( !(defined LTM_MEMCHECK_VALGRIND) && (defined LDBL_MAX))
long double ldbl_count;
#endif

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

   /* test floating point functions */
#ifdef DBL_MAX


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

   if (mp_set_double(&a, DBL_MAX) != MP_OKAY) {
      printf("\nmp_set_double(DBL_MAX) failed");
      goto LBL_ERR;
   }

   if (DBL_MAX != mp_get_double(&a)) {
      printf("\nmp_get_double(DBL_MAX) bad result! %20.20f != %20.20f \n", DBL_MAX, mp_get_double(&a));
      goto LBL_ERR;
   }
   if (mp_set_double(&a, DBL_MIN) != MP_OKAY) {
      printf("\nmp_set_double(DBL_MIN) failed");
      goto LBL_ERR;
   }
   if (0.0 != mp_get_double(&a)) {
      printf("\nmp_get_double(DBL_MIN) bad result! %20.20f != %20.20f \n", DBL_MAX, mp_get_double(&a));
      goto LBL_ERR;
   }

   dbl_count = 1.0;
   if (mp_set_double(&a, dbl_count) != MP_OKAY) {
      printf("\nmp_set_double(dbl_count) failed");
      goto LBL_ERR;
   }
   if (dbl_count != mp_get_double(&a)) {
      printf("\nmp_get_double(+dbl_count) bad result! %20.20f != %20.20f\n",
             dbl_count, mp_get_double(&a));
      goto LBL_ERR;
   }

   /*
       Test some edge cases to check for rounding errors.
       These tests may fail on e.g.: the M68K FPU architecture which
       uses more precision for computation than it says it does. That
       does cause some curious behaviour.
    */
   dbl_count = 2.0;
   for (i = 0; i < DBL_MAX_10_EXP; ++i) {
      if (mp_set_double(&a, dbl_count) != MP_OKAY) {
         printf("\nmp_set_double(dbl_count) failed");
         goto LBL_ERR;
      }
      if (dbl_count != mp_get_double(&a)) {
         printf("\nmp_get_double(+dbl_count) at i = %d bad result! %20.20f != %20.20f\n",
                i, dbl_count, mp_get_double(&a));
         goto LBL_ERR;
      }
      dbl_count = (dbl_count * 2.0);
   }
   dbl_count = 2.0;
   for (i = 0; i < DBL_MAX_10_EXP; ++i) {
      if (mp_set_double(&a, dbl_count) != MP_OKAY) {
         printf("\nmp_set_double(+dbl_count -1) failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_DOUBLE(dbl_count, mp_get_double(&a))) {
         printf("\nmp_get_double(+dbl_count - 1) at i = %d bad result! %20.20f != %20.20f\n",
                i, dbl_count, mp_get_double(&a));
         goto LBL_ERR;
      }
      dbl_count = (dbl_count * 2.0)-1;
   }
   dbl_count = 2.0;

   for (i = 0; i < DBL_MAX_10_EXP; ++i) {
      if (mp_set_double(&a, -dbl_count) != MP_OKAY) {
         printf("\nmp_set_double((-dbl_count) -1) failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_DOUBLE(-dbl_count, mp_get_double(&a))) {
         printf("\nmp_get_double((-dbl_count) - 1) at i = %d bad result! %20.20f != %20.20f\n",
                i, dbl_count, mp_get_double(&a));
         goto LBL_ERR;
      }
      dbl_count = (dbl_count * 2.0)-1;
   }

   /* Take care that everyone gets the same input */
   srand(0xdeadbeefl);
   for (i = 0; i < 1000; ++i) {
      int tmp = rand();
      double dbl = (double) tmp * rand() + 1.0;
      if (mp_set_double(&a, dbl) != MP_OKAY) {
         printf("\nmp_set_double() failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_DOUBLE(dbl, mp_get_double(&a))) {
         printf("\nmp_get_double(+dbl) at i = %d bad result! %20.20f != %20.20f ",i, -dbl, mp_get_double(&a));
         goto LBL_ERR;
      }
      if (mp_set_double(&a, -dbl) != MP_OKAY) {
         printf("\nmp_set_double() failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_DOUBLE(-dbl,mp_get_double(&a))) {
         printf("\nmp_get_double(-dbl) at i = %d bad result! %20.20f != %20.20f ",i, -dbl, mp_get_double(&a));
         goto LBL_ERR;
      }
   }

   printf("\n\nTesting: mp_get_float");
   if (mp_set_float(&a, +1.0f/0.0f) != MP_VAL) {
      printf("\nmp_set_float should return MP_VAL for +inf");
      goto LBL_ERR;
   }
   if (mp_set_float(&a, -1.0f/0.0f) != MP_VAL) {
      printf("\nmp_set_float should return MP_VAL for -inf");
      goto LBL_ERR;
   }
   if (mp_set_float(&a, +0.0f/0.0f) != MP_VAL) {
      printf("\nmp_set_float should return MP_VAL for NaN");
      goto LBL_ERR;
   }
   if (mp_set_float(&a, -0.0f/0.0f) != MP_VAL) {
      printf("\nmp_set_float should return MP_VAL for NaN");
      goto LBL_ERR;
   }

   if (mp_set_float(&a, FLT_MAX) != MP_OKAY) {
      printf("\nmp_set_float(FLT_MAX) failed");
      goto LBL_ERR;
   }

   if (!S_COMPARE_FLOAT(FLT_MAX, mp_get_float(&a))) {
      printf("\nmp_get_float(FLT_MAX) bad result! %20.20f != %20.20f \n", FLT_MAX, mp_get_float(&a));
      goto LBL_ERR;
   }
   if (mp_set_float(&a, FLT_MIN) != MP_OKAY) {
      printf("\nmp_set_float(FLT_MIN) failed");
      goto LBL_ERR;
   }
   if (!S_COMPARE_FLOAT(0.0f, mp_get_float(&a))) {
      printf("\nmp_get_float(FLT_MIN) bad result! %20.20f != %20.20f \n", FLT_MIN, mp_get_float(&a));
      goto LBL_ERR;
   }

   flt_count = 1.0f;
   if (mp_set_float(&a, flt_count) != MP_OKAY) {
      printf("\nmp_set_float(flt_count) failed 1");
      goto LBL_ERR;
   }
   if (!S_COMPARE_FLOAT(flt_count,mp_get_float(&a))) {
      printf("\nmp_get_float(+flt_count) at i = %d bad result! %20.20f != %20.20f\n",
             i, flt_count, mp_get_float(&a));
      goto LBL_ERR;
   }
   flt_count = 2.0f;
   for (i = 0; i < FLT_MAX_10_EXP; ++i) {
      if (mp_set_float(&a, flt_count) != MP_OKAY) {
         printf("\nmp_set_float(+flt_count) at i = %d bad result! %20.20f != %20.20f\n",
                i, flt_count, mp_get_float(&a));
         goto LBL_ERR;
      }
      if (!S_COMPARE_FLOAT(flt_count, mp_get_float(&a))) {
         printf("\nmp_get_float(+flt_count) at i = %d bad result! %20.20f != %20.20f\n",
                i, flt_count, mp_get_float(&a));
         goto LBL_ERR;
      }
      flt_count = (flt_count * 2.0f);
   }

   flt_count = 2.0f;
   for (i = 0; i < FLT_MAX_10_EXP; ++i) {
      if (mp_set_float(&a, flt_count) != MP_OKAY) {
         printf("\nmp_set_float(+flt_count -1) failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_FLOAT(flt_count, mp_get_float(&a))) {
         printf("\nmp_get_float(+flt_count - 1) at i = %d bad result! %20.20f != %20.20f\n",
                i, flt_count, mp_get_float(&a));
         goto LBL_ERR;
      }
      flt_count = (flt_count * 2.0f) - 1.0f;
   }

   flt_count = 2.0f;
   for (i = 0; i < FLT_MAX_10_EXP; ++i) {
      if (mp_set_float(&a, -flt_count) != MP_OKAY) {
         printf("\nmp_set_float((-flt_count) -1) failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_FLOAT(-flt_count, mp_get_float(&a))) {
         printf("\nmp_get_float((-flt_count) - 1) at i = %d bad result! %20.20f != %20.20f\n",
                i, flt_count, mp_get_float(&a));
         goto LBL_ERR;
      }
      flt_count = (flt_count * 2.0f) - 1.0f;
   }

   srand(0xdeadbeefl);
   for (i = 0; i < 1000; ++i) {
      /* Sorry for the implicit but an explicit conversion results in an error with -Wbad-function-cast */
      float dbl = rand() + 1.0f;
      if (mp_set_float(&a, dbl) != MP_OKAY) {
         printf("\nmp_set_float() failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_FLOAT(dbl, mp_get_float(&a))) {
         printf("\nmp_get_float(+dbl) at i = %d bad result! %a != %a ",i, -dbl, mp_get_float(&a));
         goto LBL_ERR;
      }
      if (mp_set_float(&a, -dbl) != MP_OKAY) {
         printf("\nmp_set_float() failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_FLOAT(-dbl,mp_get_float(&a))) {
         printf("\nmp_get_float(-dbl) at i = %d bad result! %a != %a ",i, -dbl, mp_get_float(&a));
         goto LBL_ERR;
      }
   }

   /*  Valgrind does not support "long double" sufficiently */
#ifdef LTM_MEMCHECK_VALGRIND
#  ifdef _MSC_VER
#    pragma message("The type 'long double' is not sufficiently supported by Valgrind. Tests skipped!.")
#    pragma message("Please run tests outside of valgrind, too!")
#  else
#    warning "The type 'long double' is not sufficiently supported by Valgrind. Tests skipped!."
#    warning "Please run tests outside of valgrind, too!"
#  endif
#endif
#if ( !(defined LTM_MEMCHECK_VALGRIND) && (defined LDBL_MAX))
   printf("\n\nTesting: mp_get_long_double");
   if (mp_set_long_double(&a, +1.0L/0.0L) != MP_VAL) {
      printf("\nmp_set_long_double should return MP_VAL for +inf");
      goto LBL_ERR;
   }
   if (mp_set_long_double(&a, -1.0L/0.0L) != MP_VAL) {
      printf("\nmp_set_long_double should return MP_VAL for -inf");
      goto LBL_ERR;
   }
   if (mp_set_long_double(&a, +0.0L/0.0L) != MP_VAL) {
      printf("\nmp_set_long_double should return MP_VAL for NaN");
      goto LBL_ERR;
   }
   if (mp_set_long_double(&a, -0.0L/0.0L) != MP_VAL) {
      printf("\nmp_set_long_double should return MP_VAL for NaN");
      goto LBL_ERR;
   }
   if (mp_set_long_double(&a, LDBL_MAX) != MP_OKAY) {
      printf("\nmp_set_long_double(LDBL_MAX) failed");
      goto LBL_ERR;
   }

   if (!S_COMPARE_LONG_DOUBLE(LDBL_MAX, mp_get_long_double(&a))) {
      printf("\nmp_get_long_double(LDBL_MAX) bad result! %20.20Lf != %20.20Lf \n",
             LDBL_MAX, mp_get_long_double(&a));
      goto LBL_ERR;
   }
   if (mp_set_long_double(&a, LDBL_MIN) != MP_OKAY) {
      printf("\nmp_set_long_double(LDBL_MIN) failed");
      goto LBL_ERR;
   }
   if (!S_COMPARE_LONG_DOUBLE(0.0L, mp_get_long_double(&a))) {
      printf("\nmp_get_long_double(LDBL_MIN) bad result! %20.20Lf != %20.20Lf \n",
             LDBL_MIN, mp_get_long_double(&a));
      goto LBL_ERR;
   }

   ldbl_count = 1.0L;
   if (mp_set_long_double(&a, ldbl_count) != MP_OKAY) {
      printf("\nmp_set_long_double(ldbl_count) failed");
      goto LBL_ERR;
   }
   if (!S_COMPARE_LONG_DOUBLE(ldbl_count, mp_get_long_double(&a))) {
      printf("\nmp_get_long_double(+ldbl_count) at i = %d bad result! %20.20Lf != %20.20Lf\n",
             i, ldbl_count, mp_get_long_double(&a));
      goto LBL_ERR;
   }

   ldbl_count = 2.0L;
   /*
      We need to use  LDBL_MAX_10_EXP because some compilers accept "long double"
      but use "double" instead.
    */
   for (i = 0; i < LDBL_MAX_10_EXP; ++i) {
      if (mp_set_long_double(&a, ldbl_count) != MP_OKAY) {
         printf("\nmp_set_long_double(ldbl_count) failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_LONG_DOUBLE(ldbl_count, mp_get_long_double(&a))) {
         printf("\nmp_get_long_double(+ldbl_count) at i = %d bad result! %20.20Lf != %20.20Lf\n",
                i, ldbl_count, mp_get_long_double(&a));
         goto LBL_ERR;
      }
      ldbl_count = (ldbl_count * 2.0L);
   }
   ldbl_count = 2.0L;
   for (i = 0; i < LDBL_MAX_10_EXP; ++i) {
      if (mp_set_long_double(&a, ldbl_count) != MP_OKAY) {
         printf("\nmp_set_long_double(+ldbl_count -1) failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_LONG_DOUBLE(ldbl_count, mp_get_long_double(&a))) {
         printf("\nmp_get_long_double(+ldbl_count - 1) at i = %d bad result! %20.20Lf != %20.20Lf\n",
                i, ldbl_count, mp_get_long_double(&a));
         goto LBL_ERR;
      }
      ldbl_count = (ldbl_count * 2.0L) - 1.0L;
   }
   ldbl_count = 2.0L;

   for (i = 0; i < LDBL_MAX_10_EXP; ++i) {
      if (mp_set_long_double(&a, -ldbl_count) != MP_OKAY) {
         printf("\nmp_set_long_double((-ldbl_count) -1) failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_LONG_DOUBLE(-ldbl_count, mp_get_long_double(&a))) {
         printf("\nmp_get_long_double((-ldbl_count) - 1) at i = %d bad result! %20.20Lf != %20.20Lf\n",
                i, ldbl_count, mp_get_long_double(&a));
         goto LBL_ERR;
      }
      ldbl_count = (ldbl_count * 2.0L) - 1.0L;
   }


   srand(0xdeadbeefl);
   for (i = 0; i < 1000; ++i) {
      int tmp = rand();
      long double dbl = (long double) tmp * rand() + 1.0L;
      if (mp_set_long_double(&a, dbl) != MP_OKAY) {
         printf("\nmp_set_long_double() failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_LONG_DOUBLE(dbl, mp_get_long_double(&a))) {
         printf("\nmp_get_long_double(+dbl) at i = %d bad result! %20.20Lf != %20.20Lf ",
                i, -dbl, mp_get_long_double(&a));
         goto LBL_ERR;
      }
      if (mp_set_long_double(&a, -dbl) != MP_OKAY) {
         printf("\nmp_set_long_double() failed");
         goto LBL_ERR;
      }
      if (!S_COMPARE_LONG_DOUBLE(-dbl,mp_get_long_double(&a))) {
         printf("\nmp_get_long_double(-dbl) at i = %d bad result!  %20.20Lf != %20.20Lf ",
                i, -dbl, mp_get_long_double(&a));
         goto LBL_ERR;
      }
   }
#endif
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
      T(mp_tc_and),
      T(mp_tc_div_2d),
      T(mp_tc_or),
      T(mp_tc_xor),
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
      printf("TEST %s\n\n", test[i].name);
      if (test[i].fn() != EXIT_SUCCESS) {
         printf("\n\nFAIL %s\n\n", test[i].name);
         res = EXIT_FAILURE;
         break;
      }
      printf("\n\n");
   }

#if defined(LTM_DEMO_REAL_RAND) && !defined(_WIN32)
   if (fd_urandom) {
      fclose(fd_urandom);
   }
#endif
   return res;
}
