#include <inttypes.h>
#include "shared.h"

#define S_MP_RAND_JENKINS_C
#include "s_mp_rand_jenkins.c"

static long rand_long(void)
{
   long x;
   if (s_mp_rand_jenkins(&x, sizeof(x)) != MP_OKAY) {
      fprintf(stderr, "s_mp_rand_source failed\n");
      exit(EXIT_FAILURE);
   }
   return x;
}

static int rand_int(void)
{
   int x;
   if (s_mp_rand_jenkins(&x, sizeof(x)) != MP_OKAY) {
      fprintf(stderr, "s_mp_rand_source failed\n");
      exit(EXIT_FAILURE);
   }
   return x;
}

static int32_t rand_int32(void)
{
   int32_t x;
   if (s_mp_rand_jenkins(&x, sizeof(x)) != MP_OKAY) {
      fprintf(stderr, "s_mp_rand_source failed\n");
      exit(EXIT_FAILURE);
   }
   return x;
}

static int64_t rand_int64(void)
{
   int64_t x;
   if (s_mp_rand_jenkins(&x, sizeof(x)) != MP_OKAY) {
      fprintf(stderr, "s_mp_rand_source failed\n");
      exit(EXIT_FAILURE);
   }
   return x;
}

static uint32_t uabs32(int32_t x)
{
   return (x > 0) ? (uint32_t)x : -(uint32_t)x;
}

static uint64_t uabs64(int64_t x)
{
   return (x > 0) ? (uint64_t)x : -(uint64_t)x;
}

/* This function prototype is needed
 * to test dead code elimination
 * which is used for feature detection.
 *
 * If the feature detection does not
 * work as desired we will get a linker error.
 */
void does_not_exist(void);

static int test_feature_detection(void)
{
#define TEST_FEATURE1_C
   if (!MP_HAS(TEST_FEATURE1)) {
      does_not_exist();
      return EXIT_FAILURE;
   }

#define TEST_FEATURE2_C 1
   if (MP_HAS(TEST_FEATURE2)) {
      does_not_exist();
      return EXIT_FAILURE;
   }

#define TEST_FEATURE3_C 0
   if (MP_HAS(TEST_FEATURE3)) {
      does_not_exist();
      return EXIT_FAILURE;
   }

#define TEST_FEATURE4_C something
   if (MP_HAS(TEST_FEATURE4)) {
      does_not_exist();
      return EXIT_FAILURE;
   }

   if (MP_HAS(TEST_FEATURE5)) {
      does_not_exist();
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

static int test_trivial_stuff(void)
{
   mp_int a, b, c, d;
   DOR(mp_init_multi(&a, &b, &c, &d, NULL));
   EXPECT(mp_error_to_string(MP_OKAY) != NULL);

   /* a: 0->5 */
   mp_set(&a, 5u);
   /* a: 5-> b: -5 */
   DO(mp_neg(&a, &b));
   EXPECT(mp_cmp(&a, &b) == MP_GT);
   EXPECT(mp_cmp(&b, &a) == MP_LT);
   EXPECT(mp_isneg(&b));
   /* a: 5-> a: -5 */
   DO(mp_neg(&a, &a));
   EXPECT(mp_cmp(&b, &a) == MP_EQ);
   EXPECT(mp_isneg(&a));
   /* a: -5-> b: 5 */
   DO(mp_abs(&a, &b));
   EXPECT(!mp_isneg(&b));
   /* a: -5-> b: -4 */
   DO(mp_add_d(&a, 1u, &b));
   EXPECT(mp_isneg(&b));
   EXPECT(mp_get_i32(&b) == -4);
   EXPECT(mp_get_u32(&b) == (uint32_t)-4);
   EXPECT(mp_get_mag_u32(&b) == 4);
   /* a: -5-> b: 1 */
   DO(mp_add_d(&a, 6u, &b));
   EXPECT(mp_get_u32(&b) == 1);
   /* a: -5-> a: 1 */
   DO(mp_add_d(&a, 6u, &a));
   EXPECT(mp_get_u32(&a) == 1);
   mp_zero(&a);
   /* a: 0-> a: 6 */
   DO(mp_add_d(&a, 6u, &a));
   EXPECT(mp_get_u32(&a) == 6);

   mp_set(&a, 42u);
   mp_set(&b, 1u);
   DO(mp_neg(&b, &b));
   mp_set(&c, 1u);
   DO(mp_exptmod(&a, &b, &c, &d));

   mp_set(&c, 7u);
   /* same here */
   EXPECT(mp_exptmod(&a, &b, &c, &d) != MP_OKAY);

   EXPECT(mp_iseven(&a) != mp_isodd(&a));

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int check_get_set_i32(mp_int *a, int32_t b)
{
   mp_clear(a);
   DO(mp_shrink(a));

   mp_set_i32(a, b);
   DO(mp_shrink(a));
   EXPECT(mp_get_i32(a) == b);
   EXPECT(mp_get_u32(a) == (uint32_t)b);
   EXPECT(mp_get_mag_u32(a) == uabs32(b));

   mp_set_u32(a, (uint32_t)b);
   EXPECT(mp_get_u32(a) == (uint32_t)b);
   EXPECT(mp_get_i32(a) == (int32_t)(uint32_t)b);

   return EXIT_SUCCESS;
LBL_ERR:
   return EXIT_FAILURE;
}

static int test_mp_get_set_i32(void)
{
   int i;
   mp_int a;

   DOR(mp_init(&a));

   check_get_set_i32(&a, 0);
   check_get_set_i32(&a, -1);
   check_get_set_i32(&a, 1);
   check_get_set_i32(&a, INT32_MIN);
   check_get_set_i32(&a, INT32_MAX);

   for (i = 0; i < 1000; ++i) {
      int32_t b = rand_int32();
      EXPECT(check_get_set_i32(&a, b) == EXIT_SUCCESS);
   }

   mp_clear(&a);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear(&a);
   return EXIT_FAILURE;
}

static int check_get_set_i64(mp_int *a, int64_t b)
{
   mp_clear(a);
   if (mp_shrink(a) != MP_OKAY) return EXIT_FAILURE;

   mp_set_i64(a, b);
   if (mp_shrink(a) != MP_OKAY) return EXIT_FAILURE;
   if (mp_get_i64(a) != b) return EXIT_FAILURE;
   if (mp_get_u64(a) != (uint64_t)b) return EXIT_FAILURE;
   if (mp_get_mag_u64(a) != uabs64(b)) return EXIT_FAILURE;

   mp_set_u64(a, (uint64_t)b);
   if (mp_get_u64(a) != (uint64_t)b) return EXIT_FAILURE;
   if (mp_get_i64(a) != (int64_t)(uint64_t)b) return EXIT_FAILURE;

   return EXIT_SUCCESS;
}

static int test_mp_get_set_i64(void)
{
   int i;
   mp_int a;

   DOR(mp_init(&a));

   check_get_set_i64(&a, 0);
   check_get_set_i64(&a, -1);
   check_get_set_i64(&a, 1);
   check_get_set_i64(&a, INT64_MIN);
   check_get_set_i64(&a, INT64_MAX);

   for (i = 0; i < 1000; ++i) {
      int64_t b = rand_int64();
      if (check_get_set_i64(&a, b) != EXIT_SUCCESS) {
         goto LBL_ERR;
      }
   }

   mp_clear(&a);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear(&a);
   return EXIT_FAILURE;
}

static int test_mp_fread_fwrite(void)
{
   mp_int a, b;
   FILE *tmp = NULL;
   DOR(mp_init_multi(&a, &b, NULL));

   mp_set_ul(&a, 123456uL);
   tmp = tmpfile();
   DO(mp_fwrite(&a, 64, tmp));
   rewind(tmp);
   DO(mp_fread(&b, 64, tmp));
   EXPECT(mp_get_u32(&b) == 123456uL);
   fclose(tmp);

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   if (tmp != NULL) fclose(tmp);
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static mp_err very_random_source(void *out, size_t size)
{
   memset(out, 0xff, size);
   return MP_OKAY;
}

static int test_mp_rand(void)
{
   mp_int a, b;
   int n;
   mp_err e = MP_OKAY;
   DOR(mp_init_multi(&a, &b, NULL));
   mp_rand_source(very_random_source);
   for (n = 1; n < 1024; ++n) {
      DO(mp_rand(&a, n));
      DO(mp_incr(&a));
      DO(mp_div_2d(&a, n * MP_DIGIT_BIT, &b, NULL));
      if (mp_cmp_d(&b, 1u) != MP_EQ) {
         ndraw(&a, "mp_rand() a");
         ndraw(&b, "mp_rand() b");
         e = MP_ERR;
         break;
      }
   }
LBL_ERR:
   mp_rand_source(s_mp_rand_jenkins);
   mp_clear_multi(&a, &b, NULL);
   return (e == MP_OKAY) ? EXIT_SUCCESS : EXIT_FAILURE;
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
   int i, cnt;
   mp_int a, b;
   DOR(mp_init_multi(&a, &b, NULL));

   mp_set_ul(&a, 0uL);
   mp_set_ul(&b, 1uL);
   DO(mp_kronecker(&a, &b, &i));
   EXPECT(i == 1);
   for (cnt = 0; cnt < (int)(sizeof(kronecker)/sizeof(kronecker[0])); ++cnt) {
      k = kronecker[cnt].n;
      mp_set_l(&a, k);
      /* only test positive values of a */
      for (m = -10; m <= 10; m++) {
         mp_set_l(&b, m);
         DO(mp_kronecker(&a, &b, &i));
         EXPECT(i == kronecker[cnt].c[m + 10]);
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
   DOR(mp_init_multi(&a, &b, &c, NULL));

   for (i = 0; i < 1000; ++i) {
      long l = rand_long();
      mp_set_l(&a, l);
      DO(mp_complement(&a, &b));

      l = ~l;
      mp_set_l(&c, l);

      EXPECT(mp_cmp(&b, &c) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_signed_rsh(void)
{
   int i;

   mp_int a, b, d;
   DOR(mp_init_multi(&a, &b, &d, NULL));

   for (i = 0; i < 1000; ++i) {
      long l;
      int em;

      l = rand_long();
      mp_set_l(&a, l);

      em = abs(rand_int()) % 32;

      mp_set_l(&d, l >> em);

      DO(mp_signed_rsh(&a, em, &b));
      EXPECT(mp_cmp(&b, &d) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &d, NULL);
   return EXIT_FAILURE;

}

static int test_mp_xor(void)
{
   int i;

   mp_int a, b, c, d;
   DOR(mp_init_multi(&a, &b, &c, &d, NULL));

   for (i = 0; i < 1000; ++i) {
      long l, em;

      l = rand_long();
      mp_set_l(&a,l);

      em = rand_long();
      mp_set_l(&b, em);

      mp_set_l(&d, l ^ em);

      DO(mp_xor(&a, &b, &c));
      EXPECT(mp_cmp(&c, &d) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;

}

static int test_mp_or(void)
{
   int i;

   mp_int a, b, c, d;
   DOR(mp_init_multi(&a, &b, &c, &d, NULL));

   for (i = 0; i < 1000; ++i) {
      long l, em;

      l = rand_long();
      mp_set_l(&a, l);

      em = rand_long();
      mp_set_l(&b, em);

      mp_set_l(&d, l | em);

      DO(mp_or(&a, &b, &c));
      EXPECT(mp_cmp(&c, &d) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int test_mp_and(void)
{
   int i;

   mp_int a, b, c, d;
   DOR(mp_init_multi(&a, &b, &c, &d, NULL));

   for (i = 0; i < 1000; ++i) {
      long l, em;

      l = rand_long();
      mp_set_l(&a, l);

      em = rand_long();
      mp_set_l(&b, em);

      mp_set_l(&d, l & em);

      DO(mp_and(&a, &b, &c));
      EXPECT(mp_cmp(&c, &d) == MP_EQ);
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
   DOR(mp_init_multi(&a, &b, &c, &d, NULL));

   /* mp_invmod corner-case of https://github.com/libtom/libtommath/issues/118 */
   {
      const char *a_ = "47182BB8DF0FFE9F61B1F269BACC066B48BA145D35137D426328DC3F88A5EA44";
      const char *b_ = "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF";
      const char *should_ = "0521A82E10376F8E4FDEF9A32A427AC2A0FFF686E00290D39E3E4B5522409596";

      DO(mp_read_radix(&a, a_, 16));
      DO(mp_read_radix(&b, b_, 16));
      DO(mp_read_radix(&c, should_, 16));
      DO(mp_invmod(&a, &b, &d));
      EXPECT(mp_cmp(&c, &d) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;

}

#if defined(MP_HAS_SET_DOUBLE)

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4723) /* potential divide by 0 */
#endif
static int test_mp_set_double(void)
{
   int i;
   double dbl_zero = 0.0;

   mp_int a, b;
   DOR(mp_init_multi(&a, &b, NULL));
   /* test mp_get_double/mp_set_double */
   EXPECT(mp_set_double(&a, +1.0/dbl_zero) == MP_VAL);
   EXPECT(mp_set_double(&a, -1.0/dbl_zero) == MP_VAL);
   EXPECT(mp_set_double(&a, +0.0/dbl_zero) == MP_VAL);
   EXPECT(mp_set_double(&a, -0.0/dbl_zero) == MP_VAL);

   for (i = 0; i < 1000; ++i) {
      int tmp = rand_int();
      double dbl = (double)tmp * rand_int() + 1;
      DO(mp_set_double(&a, dbl));
      EXPECT(dbl == mp_get_double(&a));
      DO(mp_set_double(&a, -dbl));
      EXPECT(-dbl == mp_get_double(&a));
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;

}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif

static int test_mp_get_u32(void)
{
   uint32_t t;
   int i;

   mp_int a, b;
   DOR(mp_init_multi(&a, &b, NULL));

   for (i = 0; i < 1000; ++i) {
      t = (uint32_t)rand_long();
      mp_set_ul(&a, t);
      EXPECT(t == mp_get_u32(&a));
   }
   mp_set_ul(&a, 0uL);
   EXPECT(mp_get_u32(&a) == 0);
   mp_set_ul(&a, UINT32_MAX);
   EXPECT(mp_get_u32(&a) == UINT32_MAX);

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_get_ul(void)
{
   unsigned long s, t;
   int i;

   mp_int a, b;
   DOR(mp_init_multi(&a, &b, NULL));

   for (i = 0; i < ((int)MP_SIZEOF_BITS(unsigned long) - 1); ++i) {
      t = (1UL << (i+1)) - 1;
      if (!t)
         t = ~0UL;
      printf(" t = 0x%lx i = %d\r", t, i);
      do {
         mp_set_ul(&a, t);
         s = mp_get_ul(&a);
         EXPECT(s == t);
         t <<= 1;
      } while (t != 0uL);
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_get_u64(void)
{
   uint64_t q, r;
   int i;

   mp_int a, b;
   DOR(mp_init_multi(&a, &b, NULL));

   for (i = 0; i < (int)(MP_SIZEOF_BITS(uint64_t) - 1); ++i) {
      r = ((uint64_t)1 << (i+1)) - 1;
      if (!r)
         r = UINT64_MAX;
      printf(" r = 0x%" PRIx64 " i = %d\r", r, i);
      do {
         mp_set_u64(&a, r);
         q = mp_get_u64(&a);
         EXPECT(q == r);
         r <<= 1;
      } while (r != 0u);
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

   mp_int a, b, c;
   DOR(mp_init_multi(&a, &b, &c, NULL));

   for (i = 0; i < 1000; ++i) {
      printf("%6d\r", i);
      fflush(stdout);
      n = (rand_int() & 15) + 1;
      DO(mp_rand(&a, n));
      DO(mp_sqrt(&a, &b));
      DO(mp_root_n(&a, 2, &c));
      EXPECT(mp_cmp_mag(&b, &c) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_is_square(void)
{
   int i, n;

   mp_int a, b;
   bool res;

   DOR(mp_init_multi(&a, &b, NULL));

   for (i = 0; i < 1000; ++i) {
      printf("%6d\r", i);
      fflush(stdout);

      /* test mp_is_square false negatives */
      n = (rand_int() & 7) + 1;
      DO(mp_rand(&a, n));
      DO(mp_sqr(&a, &a));
      DO(mp_is_square(&a, &res));
      EXPECT(res);

      /* test for false positives */
      DO(mp_add_d(&a, 1u, &a));
      DO(mp_is_square(&a, &res));
      EXPECT(!res);
   }

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
      { 5, 14, 3 },   /* 5 \cong 1 (mod 4) */
      { 7, 9, 4 },    /* 7 \cong 3 (mod 4) */
      { 113, 2, 62 }  /* 113 \cong 1 (mod 4) */
   };
   int i;

   mp_int a, b, c;
   DOR(mp_init_multi(&a, &b, &c, NULL));

   /* r^2 = n (mod p) */
   for (i = 0; i < (int)(sizeof(sqrtmod_prime)/sizeof(sqrtmod_prime[0])); ++i) {
      mp_set_ul(&a, sqrtmod_prime[i].p);
      mp_set_ul(&b, sqrtmod_prime[i].n);
      DO(mp_sqrtmod_prime(&b, &a, &c));
      EXPECT(mp_cmp_d(&c, sqrtmod_prime[i].r) == MP_EQ);
   }
   /* Check handling of wrong input (here: modulus is square and cong. 1 mod 4,24 ) */
   mp_set_ul(&a, 25);
   mp_set_ul(&b, 2);
   EXPECT(mp_sqrtmod_prime(&b, &a, &c) == MP_VAL);
   /* b \cong 0 (mod a) */
   mp_set_ul(&a, 45);
   mp_set_ul(&b, 3);
   EXPECT(mp_sqrtmod_prime(&b, &a, &c) == MP_VAL);

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_prime_rand(void)
{
   int ix;
   mp_int a, b;
   DOR(mp_init_multi(&a, &b, NULL));

   /* test for size */
   for (ix = 10; ix < 128; ix++) {
      printf("Testing (not safe-prime): %9d bits    \n", ix);
      fflush(stdout);
      DO(mp_prime_rand(&a, 8, ix, (rand_int() & 1) ? 0 : MP_PRIME_2MSB_ON));
      EXPECT(mp_count_bits(&a) == ix);
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_prime_is_prime(void)
{
   int ix;
   mp_err e;
   bool cnt, fu;

   mp_int a, b;
   DOR(mp_init_multi(&a, &b, NULL));

   /* strong Miller-Rabin pseudoprime to the first 200 primes (F. Arnault) */
   printf("Testing mp_prime_is_prime() with Arnault's pseudoprime  803...901");
   DO(mp_read_radix(&a,
                    "91xLNF3roobhzgTzoFIG6P13ZqhOVYSN60Fa7Cj2jVR1g0k89zdahO9/kAiRprpfO1VAp1aBHucLFV/qLKLFb+zonV7R2Vxp1K13ClwUXStpV0oxTNQVjwybmFb5NBEHImZ6V7P6+udRJuH8VbMEnS0H8/pSqQrg82OoQQ2fPpAk6G1hkjqoCv5s/Yr",
                    64));
   DO(mp_prime_is_prime(&a, mp_prime_rabin_miller_trials(mp_count_bits(&a)), &cnt));
   if (cnt) {
      printf("Arnault's pseudoprime is not prime but mp_prime_is_prime says it is.\n");
      goto LBL_ERR;
   }
   /* About the same size as Arnault's pseudoprime */
   printf("\rTesting mp_prime_is_prime() with certified prime 2^1119 + 53       ");
   mp_set(&a, 1u);
   DO(mp_mul_2d(&a,1119,&a));
   DO(mp_add_d(&a, 53u, &a));
   e = mp_prime_is_prime(&a, mp_prime_rabin_miller_trials(mp_count_bits(&a)), &cnt);
   /* small problem */
   if (e != MP_OKAY) {
      printf("\nfailed with error: %s\n", mp_error_to_string(e));
   }
   /* large problem */
   if (!cnt) {
      printf("A certified prime is a prime but mp_prime_is_prime says it is not.\n");
   }
   if ((e != MP_OKAY) || !cnt) {
      printf("prime tested was: 0x");
      DO(mp_fwrite(&a,16,stdout));
      putchar('\n');
      goto LBL_ERR;
   }
   printf("\r                                                                   ");

   for (ix = 16; ix < 128; ix++) {
      printf("\rTesting (    safe-prime): %9d bits    ", ix);
      fflush(stdout);
      DO(mp_prime_rand(&a, 8, ix, ((rand_int() & 1) ? 0 : MP_PRIME_2MSB_ON) | MP_PRIME_SAFE));
      EXPECT(mp_count_bits(&a) == ix);
      /* let's see if it's really a safe prime */
      DO(mp_sub_d(&a, 1u, &b));
      DO(mp_div_2(&b, &b));
      DO(mp_prime_is_prime(&b, mp_prime_rabin_miller_trials(mp_count_bits(&b)), &cnt));
      /* large problem */
      EXPECT(cnt);
      DO(mp_prime_frobenius_underwood(&b, &fu));
      EXPECT(fu);
      if ((e != MP_OKAY) || !cnt) {
         printf("prime tested was: 0x");
         DO(mp_fwrite(&a,16,stdout));
         putchar('\n');
         printf("sub tested was: 0x");
         DO(mp_fwrite(&b,16,stdout));
         putchar('\n');
         goto LBL_ERR;
      }

   }
   /* Check regarding problem #143 */
   DO(mp_read_radix(&a,
                    "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9A63A3620FFFFFFFFFFFFFFFF",
                    16));
   DO(mp_prime_strong_lucas_selfridge(&a, &cnt));
   /* large problem */
   EXPECT(cnt);
   if ((e != MP_OKAY) || !cnt) {
      printf("prime tested was: 0x");
      DO(mp_fwrite(&a,16,stdout));
      putchar('\n');
      goto LBL_ERR;
   }

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;

}


static int test_mp_prime_next_prime(void)
{
   mp_int a, b, c;

   DOR(mp_init_multi(&a, &b, &c, NULL));


   /* edge cases */
   mp_set(&a, 0u);
   DO(mp_prime_next_prime(&a, 5, false));
   if (mp_cmp_d(&a, 2u) != MP_EQ) {
      printf("mp_prime_next_prime: output should have been 2 but was: ");
      DO(mp_fwrite(&a,10,stdout));
      putchar('\n');
      goto LBL_ERR;
   }

   mp_set(&a, 0u);
   DO(mp_prime_next_prime(&a, 5, true));
   if (mp_cmp_d(&a, 3u) != MP_EQ) {
      printf("mp_prime_next_prime: output should have been 3 but was: ");
      DO(mp_fwrite(&a,10,stdout));
      putchar('\n');
      goto LBL_ERR;
   }

   mp_set(&a, 2u);
   DO(mp_prime_next_prime(&a, 5, false));
   if (mp_cmp_d(&a, 3u) != MP_EQ) {
      printf("mp_prime_next_prime: output should have been 3 but was: ");
      DO(mp_fwrite(&a,10,stdout));
      putchar('\n');
      goto LBL_ERR;
   }

   mp_set(&a, 2u);
   DO(mp_prime_next_prime(&a, 5, true));
   if (mp_cmp_d(&a, 3u) != MP_EQ) {
      printf("mp_prime_next_prime: output should have been 3 but was: ");
      DO(mp_fwrite(&a,10,stdout));
      putchar('\n');
      goto LBL_ERR;
   }
   mp_set(&a, 8u);
   DO(mp_prime_next_prime(&a, 5, true));
   if (mp_cmp_d(&a, 11u) != MP_EQ) {
      printf("mp_prime_next_prime: output should have been 11 but was: ");
      DO(mp_fwrite(&a,10,stdout));
      putchar('\n');
      goto LBL_ERR;
   }
   /* 2^300 + 157 is a 300 bit large prime to guarantee a multi-limb bigint */
   DO(mp_2expt(&a, 300));
   mp_set_u32(&b, 157);
   DO(mp_add(&a, &b, &a));
   DO(mp_copy(&a, &b));

   /* 2^300 + 385 is the next prime */
   mp_set_u32(&c, 228);
   DO(mp_add(&b, &c, &b));
   DO(mp_prime_next_prime(&a, 5, false));
   if (mp_cmp(&a, &b) != MP_EQ) {
      printf("mp_prime_next_prime: output should have been\n");
      DO(mp_fwrite(&b,10,stdout));
      putchar('\n');
      printf("but was:\n");
      DO(mp_fwrite(&a,10,stdout));
      putchar('\n');
      goto LBL_ERR;
   }

   /* Use another temporary variable or recompute? Mmh... */
   DO(mp_2expt(&a, 300));
   mp_set_u32(&b, 157);
   DO(mp_add(&a, &b, &a));
   DO(mp_copy(&a, &b));

   /* 2^300 + 631 is the next prime congruent to 3 mod 4*/
   mp_set_u32(&c, 474);
   DO(mp_add(&b, &c, &b));
   DO(mp_prime_next_prime(&a, 5, true));
   if (mp_cmp(&a, &b) != MP_EQ) {
      printf("mp_prime_next_prime (bbs): output should have been\n");
      DO(mp_fwrite(&b,10,stdout));
      putchar('\n');
      printf("but was:\n");
      DO(mp_fwrite(&a,10,stdout));
      putchar('\n');
      goto LBL_ERR;
   }

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_montgomery_reduce(void)
{
   mp_digit mp;
   int ix, i, n;
   char buf[4096];

   /* size_t written; */

   mp_int a, b, c, d, e;
   DOR(mp_init_multi(&a, &b, &c, &d, &e, NULL));

   /* test montgomery */
   for (i = 1; i <= 10; i++) {
      if (i == 10)
         i = 1000;
      printf(" digit size: %2d\r", i);
      fflush(stdout);
      for (n = 0; n < 1000; n++) {
         DO(mp_rand(&a, i));
         a.dp[0] |= 1;

         /* let's see if R is right */
         DO(mp_montgomery_calc_normalization(&b, &a));
         DO(mp_montgomery_setup(&a, &mp));

         /* now test a random reduction */
         for (ix = 0; ix < 100; ix++) {
            DO(mp_rand(&c, 1 + (abs(rand_int()) % (2*i))));
            DO(mp_copy(&c, &d));
            DO(mp_copy(&c, &e));

            DO(mp_mod(&d, &a, &d));
            DO(mp_montgomery_reduce(&c, &a, mp));
            DO(mp_mulmod(&c, &b, &a, &c));

            if (mp_cmp(&c, &d) != MP_EQ) {
/* *INDENT-OFF* */
               printf("d = e mod a, c = e MOD a\n");
               DO(mp_to_decimal(&a, buf, sizeof(buf))); printf("a = %s\n", buf);
               DO(mp_to_decimal(&e, buf, sizeof(buf))); printf("e = %s\n", buf);
               DO(mp_to_decimal(&d, buf, sizeof(buf))); printf("d = %s\n", buf);
               DO(mp_to_decimal(&c, buf, sizeof(buf))); printf("c = %s\n", buf);

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

   mp_clear_multi(&a, &b, &c, &d, &e, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, &e, NULL);
   return EXIT_FAILURE;

}

static int test_mp_read_radix(void)
{
   char buf[4096];
   size_t written;

   mp_int a;
   DOR(mp_init_multi(&a, NULL));

   DO(mp_read_radix(&a, "123456", 10));

   DO(mp_to_radix(&a, buf, sizeof(buf), &written, 10));
   printf(" '123456' a == %s, length = %zu", buf, written);

   /* See comment in mp_to_radix.c */
   /*
      if( (err = mp_to_radix(&a, buf, 3u, &written, 10) ) != MP_OKAY)              goto LBL_ERR;
      printf(" '56' a == %s, length = %zu\n", buf, written);

      if( (err = mp_to_radix(&a, buf, 4u, &written, 10) ) != MP_OKAY)              goto LBL_ERR;
      printf(" '456' a == %s, length = %zu\n", buf, written);
      if( (err = mp_to_radix(&a, buf, 30u, &written, 10) ) != MP_OKAY)             goto LBL_ERR;
      printf(" '123456' a == %s, length = %zu, error = %s\n",
             buf, written, mp_error_to_string(err));
   */
   DO(mp_read_radix(&a, "-123456", 10));
   DO(mp_to_radix(&a, buf, sizeof(buf), &written, 10));
   printf("\r '-123456' a == %s, length = %zu", buf, written);

   DO(mp_read_radix(&a, "0", 10));
   DO(mp_to_radix(&a, buf, sizeof(buf), &written, 10));
   printf("\r '0' a == %s, length = %zu", buf, written);

   while (0) {
      char *s = fgets(buf, sizeof(buf), stdin);
      if (s != buf) break;
      DO(mp_read_radix(&a, buf, 10));
      DO(mp_prime_next_prime(&a, 5, true));
      DO(mp_to_radix(&a, buf, sizeof(buf), NULL, 10));
      printf("%s, %lu\n", buf, (unsigned long)a.dp[0] & 3uL);
   }

   mp_clear(&a);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear(&a);
   return EXIT_FAILURE;
}

static int test_mp_cnt_lsb(void)
{
   int ix;

   mp_int a, b;
   DOR(mp_init_multi(&a, &b, NULL));

   mp_set(&a, 1u);
   for (ix = 0; ix < 1024; ix++) {
      EXPECT(mp_cnt_lsb(&a) == ix);
      DO(mp_mul_2(&a, &a));
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
   DOR(mp_init_multi(&a, &b, &c, &d, NULL));

   /* test mp_reduce_2k */
   for (cnt = 3; cnt <= 128; ++cnt) {
      mp_digit tmp;

      DO(mp_2expt(&a, cnt));
      DO(mp_sub_d(&a, 2u, &a));  /* a = 2**cnt - 2 */

      printf("\r %4d bits", cnt);
      printf("(%d)", mp_reduce_is_2k(&a));
      DO(mp_reduce_2k_setup(&a, &tmp));
      printf("(%lu)", (unsigned long) tmp);
      for (ix = 0; ix < 1000; ix++) {
         if (!(ix & 127)) {
            printf(".");
            fflush(stdout);
         }
         DO(mp_rand(&b, ((cnt / MP_DIGIT_BIT) + 1) * 2));
         DO(mp_copy(&c, &b));
         DO(mp_mod(&c, &a, &c));
         DO(mp_reduce_2k(&b, &a, 2u));
         EXPECT(mp_cmp(&c, &b) == MP_EQ);
      }
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int test_s_mp_div_3(void)
{
   int cnt;

   mp_int a, b, c, d, e;
   DOR(mp_init_multi(&a, &b, &c, &d, &e, NULL));

   /* test s_mp_div_3  */
   mp_set(&d, 3u);
   for (cnt = 0; cnt < 10000;) {
      mp_digit r2;

      if (!(++cnt & 127)) {
         printf("\r %9d", cnt);
         fflush(stdout);
      }
      DO(mp_rand(&a, (abs(rand_int()) % 128) + 1));
      DO(mp_div(&a, &d, &b, &e));
      DO(s_mp_div_3(&a, &c, &r2));

      EXPECT(!mp_cmp(&b, &c) && !mp_cmp_d(&e, r2));
   }
   printf("... passed!");

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
   DOR(mp_init_multi(&a, &b, &c, NULL));

   /* test the DR reduction */
   for (cnt = 2; cnt < 32; cnt++) {
      printf("\r%d digit modulus", cnt);
      DO(mp_grow(&a, cnt));
      mp_zero(&a);
      for (ix = 1; ix < cnt; ix++) {
         a.dp[ix] = MP_MASK;
      }
      a.used = cnt;
      a.dp[0] = 3;

      DO(mp_rand(&b, cnt - 1));
      DO(mp_copy(&b, &c));

      rr = 0;
      do {
         if (!(rr & 127)) {
            printf(".");
            fflush(stdout);
         }
         DO(mp_sqr(&b, &b));
         DO(mp_add_d(&b, 1u, &b));
         DO(mp_copy(&b, &c));

         DO(mp_mod(&b, &a, &b));
         mp_dr_setup(&a, &mp);
         DO(mp_dr_reduce(&c, &a, mp));

         EXPECT(mp_cmp(&b, &c) == MP_EQ);
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
   mp_int a, b, c, d;
   int cnt;
   char buf[4096];
   size_t length;
   DOR(mp_init_multi(&a, &b, NULL));
   /* test the mp_reduce_2k_l code */
#      if LTM_DEMO_TEST_REDUCE_2K_L == 1
   /* first load P with 2^1024 - 0x2A434 B9FDEC95 D8F9D550 FFFFFFFF FFFFFFFF */
   DO(mp_2expt(&a, 1024));
   DO(mp_read_radix(&b, "2A434B9FDEC95D8F9D550FFFFFFFFFFFFFFFF", 16));
   DO(mp_sub(&a, &b, &a));
#      elif LTM_DEMO_TEST_REDUCE_2K_L == 2
   /*  p = 2^2048 - 0x1 00000000 00000000 00000000 00000000 4945DDBF 8EA2A91D 5776399B B83E188F  */
   DO(mp_2expt(&a, 2048));
   DO(mp_read_radix(&b,
                    "1000000000000000000000000000000004945DDBF8EA2A91D5776399BB83E188F",
                    16));
   DO(mp_sub(&a, &b, &a));
#      else
#         error oops
#      endif
   DO(mp_to_radix(&a, buf, sizeof(buf), &length, 10));
   printf("\n\np==%s, length = %zu\n", buf, length);
   /* now mp_reduce_is_2k_l() should return */
   EXPECT(mp_reduce_is_2k_l(&a) == 1);
   DO(mp_reduce_2k_setup_l(&a, &d));
   /* now do a million square+1 to see if it varies */
   DO(mp_rand(&b, 64));
   DO(mp_mod(&b, &a, &b));
   DO(mp_copy(&b, &c));
   printf("Testing: mp_reduce_2k_l...");
   fflush(stdout);
   for (cnt = 0; cnt < (int)(1uL << 20); cnt++) {
      DO(mp_sqr(&b, &b));
      DO(mp_add_d(&b, 1u, &b));
      DO(mp_reduce_2k_l(&b, &a, &d));
      DO(mp_sqr(&c, &c));
      DO(mp_add_d(&c, 1u, &c));
      DO(mp_mod(&c, &a, &c));
      if (mp_cmp(&b, &c) != MP_EQ) {
         printf("mp_reduce_2k_l() failed at step %d\n", cnt);
         DO(mp_to_hex(&b, buf, sizeof(buf)));
         printf("b == %s\n", buf);
         DO(mp_to_hex(&c, buf, sizeof(buf)));
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
/* stripped down version of mp_radix_size. The faster version can be off by up t
o +3  */
static mp_err s_rs(const mp_int *a, int radix, int *size)
{
   mp_err res;
   int digs = 0u;
   mp_int  t;
   mp_digit d;
   *size = 0u;
   if (mp_iszero(a)) {
      *size = 2u;
      return MP_OKAY;
   }
   if (radix == 2) {
      *size = mp_count_bits(a) + 1;
      return MP_OKAY;
   }
   DO_WHAT(mp_init_copy(&t, a), return MP_ERR);
   t.sign = MP_ZPOS;
   while (!mp_iszero(&t)) {
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
static int test_mp_log_n(void)
{
   mp_int a;
   mp_digit d;
   int base, lb, size;
   const int max_base = MP_MIN(INT_MAX, MP_DIGIT_MAX);

   DOR(mp_init(&a));

   /*
     base   a    result
      0     x    MP_VAL
      1     x    MP_VAL
   */
   mp_set(&a, 42u);
   base = 0u;
   EXPECT(mp_log_n(&a, base, &lb) == MP_VAL);
   base = 1u;
   EXPECT(mp_log_n(&a, base, &lb) == MP_VAL);
   /*
     base   a    result
      2     0    MP_VAL
      2     1    0
      2     2    1
      2     3    1
   */
   base = 2u;
   mp_zero(&a);
   EXPECT(mp_log_n(&a, base, &lb) == MP_VAL);

   for (d = 1; d < 4; d++) {
      mp_set(&a, d);
      DO(mp_log_n(&a, base, &lb));
      EXPECT(lb == ((d == 1)?0:1));
   }
   /*
    base   a    result
     3     0    MP_VAL
     3     1    0
     3     2    0
     3     3    1
   */
   base = 3u;
   mp_zero(&a);
   EXPECT(mp_log_n(&a, base, &lb) == MP_VAL);
   for (d = 1; d < 4; d++) {
      mp_set(&a, d);
      DO(mp_log_n(&a, base, &lb));
      EXPECT(lb == (((int)d < base)?0:1));
   }

   /*
     bases 2..64 with "a" a random large constant.
     The range of bases tested allows to check with
     radix_size.
   */
   DO(mp_rand(&a, 10));
   for (base = 2; base < 65; base++) {
      DO(mp_log_n(&a, base, &lb));
      DO(s_rs(&a,(int)base, &size));
      /* radix_size includes the memory needed for '\0', too*/
      size -= 2;
      EXPECT(lb == size);
   }

   /*
     bases 2..64 with "a" a random small constant to
     test the part of mp_ilogb that uses native types.
   */
   DO(mp_rand(&a, 1));
   for (base = 2; base < 65; base++) {
      DO(mp_log_n(&a, base, &lb));
      DO(s_rs(&a,(int)base, &size));
      size -= 2;
      EXPECT(lb == size);
   }

   /*Test upper edgecase with base UINT32_MAX and number (UINT32_MAX/2)*UINT32_MAX^10  */
   mp_set(&a, max_base);
   DO(mp_expt_n(&a, 10uL, &a));
   DO(mp_add_d(&a, max_base / 2, &a));
   DO(mp_log_n(&a, max_base, &lb));
   EXPECT(lb == 10u);

   mp_clear(&a);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear(&a);
   return EXIT_FAILURE;
}

static int test_mp_incr(void)
{
   mp_int a, b;

   DOR(mp_init_multi(&a, &b, NULL));

   /* Does it increment inside the limits of a MP_xBIT limb? */
   mp_set(&a, MP_MASK/2);
   DO(mp_incr(&a));
   EXPECT(mp_cmp_d(&a, (MP_MASK/2u) + 1u) == MP_EQ);

   /* Does it increment outside of the limits of a MP_xBIT limb? */
   mp_set(&a, MP_MASK);
   mp_set(&b, MP_MASK);
   DO(mp_incr(&a));
   DO(mp_add_d(&b, 1u, &b));
   EXPECT(mp_cmp(&a, &b) == MP_EQ);

   /* Does it increment from -1 to 0? */
   mp_set(&a, 1u);
   a.sign = MP_NEG;
   DO(mp_incr(&a));
   EXPECT(mp_cmp_d(&a, 0u) == MP_EQ);

   /* Does it increment from -(MP_MASK + 1) to -MP_MASK? */
   mp_set(&a, MP_MASK);
   DO(mp_add_d(&a, 1u, &a));
   a.sign = MP_NEG;
   DO(mp_incr(&a));
   EXPECT(a.sign == MP_NEG);

   a.sign = MP_ZPOS;
   EXPECT(mp_cmp_d(&a, MP_MASK) == MP_EQ);

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

static int test_mp_decr(void)
{
   mp_int a, b;

   DOR(mp_init_multi(&a, &b, NULL));

   /* Does it decrement inside the limits of a MP_xBIT limb? */
   mp_set(&a, MP_MASK/2);
   DO(mp_decr(&a));
   EXPECT(mp_cmp_d(&a, (MP_MASK/2u) - 1u) == MP_EQ);

   /* Does it decrement outside of the limits of a MP_xBIT limb? */
   mp_set(&a, MP_MASK);
   DO(mp_add_d(&a, 1u, &a));
   DO(mp_decr(&a));
   EXPECT(mp_cmp_d(&a, MP_MASK) == MP_EQ);

   /* Does it decrement from 0 to -1? */
   mp_zero(&a);
   DO(mp_decr(&a));
   if (a.sign == MP_NEG) {
      a.sign = MP_ZPOS;
      EXPECT(mp_cmp_d(&a, 1u) == MP_EQ);
   } else {
      goto LBL_ERR;
   }


   /* Does it decrement from -MP_MASK to -(MP_MASK + 1)? */
   mp_set(&a, MP_MASK);
   a.sign = MP_NEG;
   mp_set(&b, MP_MASK);
   b.sign = MP_NEG;
   DO(mp_sub_d(&b, 1u, &b));
   DO(mp_decr(&a));
   EXPECT(mp_cmp(&a, &b) == MP_EQ);

   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

/*
   Cannot test mp_exp(_d) without mp_root_n and vice versa.
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

static int test_mp_root_n(void)
{
   mp_int a, c, r;
   int i, j;

   const char *input[] = {
      "4n9cbk886QtLQmofprid3l2Q0GD8Yv979Lh8BdZkFE8g2pDUUSMBET/+M/YFyVZ3mBp",
      "5NlgzHhmIX05O5YoW5yW5reAlVNtRAlIcN2dfoATnNdc1Cw5lHZUTwNthmK6/ZLKfY6",
      "3gweiHDX+ji5utraSe46IJX+uuh7iggs63xIpMP5MriU4Np+LpHI5are8RzS9pKh9xP",
      "5QOJUSKMrfe7LkeyJOlupS8h7bjT+TXmZkDzOjZtfj7mdA7cbg0lRX3CuafhjIrpK8S",
      "4HtYFldVkyVbrlg/s7kmaA7j45PvLQm+1bbn6ehgP8tVoBmGbv2yDQI1iQQze4AlHyN",
      "3bwCUx79NAR7c68OPSp5ZabhZ9aBEr7rWNTO2oMY7zhbbbw7p6shSMxqE9K9nrTNucf",
      "4j5RGb78TfuYSzrXn0z6tiAoWiRI81hGY3el9AEa9S+gN4x/AmzotHT2Hvj6lyBpE7q",
      "4lwg30SXqZhEHNsl5LIXdyu7UNt0VTWebP3m7+WUL+hsnFW9xJe7UnzYngZsvWh14IE",
      "1+tcqFeRuGqjRADRoRUJ8gL4UUSFQVrVVoV6JpwVcKsuBq5G0pABn0dLcQQQMViiVRj",
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

   DOR(mp_init_multi(&a, &c, &r, NULL));
   for (i = 0; i < 10; i++) {
      DO(mp_read_radix(&a, input[i], 64));
      for (j = 3; j < 100; j++) {
         DO(mp_root_n(&a, j, &c));
         DO(mp_read_radix(&r, root[i][j-3], 10));
         EXPECT(mp_cmp(&r, &c) == MP_EQ);
      }
   }
   mp_clear_multi(&a, &c, &r, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &c, &r, NULL);
   return EXIT_FAILURE;
}

static int test_s_mp_mul_balance(void)
{
   mp_int a, b, c;

   const char *na =
      "4b0I5uMTujCysw+1OOuOyH2FX2WymrHUqi8BBDb7XpkV/4i7vXTbEYUy/kdIfCKu5jT5JEqYkdmnn3jAYo8XShPzNLxZx9yoLjxYRyptSuOI2B1DspvbIVYXY12sxPZ4/HCJ4Usm2MU5lO/006KnDMxuxiv1rm6YZJZ0eZU";
   const char *nb = "3x9vs0yVi4hIq7poAeVcggC3WoRt0zRLKO";
   const char *nc =
      "HzrSq9WVt1jDTVlwUxSKqxctu2GVD+N8+SVGaPFRqdxyld6IxDBbj27BPJzYUdR96k3sWpkO8XnDBvupGPnehpQe4KlO/KmN1PjFov/UTZYM+LYzkFcBPyV6hkkL8ePC1rlFLAHzgJMBCXVp4mRqtkQrDsZXXlcqlbTFu69wF6zDEysiX2cAtn/kP9ldblJiwYPCD8hG";

   DOR(mp_init_multi(&a, &b, &c, NULL));

   DO(mp_read_radix(&a, na, 64));
   DO(mp_read_radix(&b, nb, 64));

   DO(s_mp_mul_balance(&a, &b, &c));

   DO(mp_read_radix(&b, nc, 64));

   EXPECT(mp_cmp(&b, &c) == MP_EQ);

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

#define s_mp_mul_full(a, b, c) s_mp_mul(a, b, c, (a)->used + (b)->used + 1)
static int test_s_mp_mul_karatsuba(void)
{
   mp_int a, b, c, d;
   int size;

   DOR(mp_init_multi(&a, &b, &c, &d, NULL));
   for (size = MP_MUL_KARATSUBA_CUTOFF; size < (MP_MUL_KARATSUBA_CUTOFF + 20); size++) {
      DO(mp_rand(&a, size));
      DO(mp_rand(&b, size));
      DO(s_mp_mul_karatsuba(&a, &b, &c));
      DO(s_mp_mul_full(&a,&b,&d));
      EXPECT(mp_cmp(&c, &d) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int test_s_mp_sqr_karatsuba(void)
{
   mp_int a, b, c;
   int size;

   DOR(mp_init_multi(&a, &b, &c, NULL));
   for (size = MP_SQR_KARATSUBA_CUTOFF; size < (MP_SQR_KARATSUBA_CUTOFF + 20); size++) {
      DO(mp_rand(&a, size));
      DO(s_mp_sqr_karatsuba(&a, &b));
      DO(s_mp_sqr(&a, &c));
      EXPECT(mp_cmp(&b, &c) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_s_mp_mul_toom(void)
{
   mp_int a, b, c, d;
   int size;

#if (MP_DIGIT_BIT == 60)
   int tc_cutoff;
#endif

   DOR(mp_init_multi(&a, &b, &c, &d, NULL));
   /* This number construction is limb-size specific */
#if (MP_DIGIT_BIT == 60)
   DO(mp_rand(&a, 1196));
   DO(mp_mul_2d(&a,71787  - mp_count_bits(&a), &a));

   DO(mp_rand(&b, 1338));
   DO(mp_mul_2d(&b, 80318 - mp_count_bits(&b), &b));
   DO(mp_mul_2d(&b, 6310, &b));
   DO(mp_2expt(&c, 99000 - 1000));
   DO(mp_add(&b, &c, &b));

   tc_cutoff = MP_MUL_TOOM_CUTOFF;
   MP_MUL_TOOM_CUTOFF = INT_MAX;
   DO(mp_mul(&a, &b, &c));
   MP_MUL_TOOM_CUTOFF = tc_cutoff;
   DO(mp_mul(&a, &b, &d));
   EXPECT(mp_cmp(&c, &d) == MP_EQ);
#endif

   for (size = MP_MUL_TOOM_CUTOFF; size < (MP_MUL_TOOM_CUTOFF + 20); size++) {
      DO(mp_rand(&a, size));
      DO(mp_rand(&b, size));
      DO(s_mp_mul_toom(&a, &b, &c));
      DO(s_mp_mul_full(&a,&b,&d));
      EXPECT(mp_cmp(&c, &d) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return EXIT_FAILURE;
}

static int test_s_mp_sqr_toom(void)
{
   mp_int a, b, c;
   int size;

   DOR(mp_init_multi(&a, &b, &c, NULL));
   for (size = MP_SQR_TOOM_CUTOFF; size < (MP_SQR_TOOM_CUTOFF + 20); size++) {
      DO(mp_rand(&a, size));
      DO(s_mp_sqr_toom(&a, &b));
      DO(s_mp_sqr(&a, &c));
      EXPECT(mp_cmp(&b, &c) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}


static int test_mp_radix_size(void)
{
   mp_int a;
   int radix;
   size_t size;
/* *INDENT-OFF* */
   size_t results[65] = {
       0, 0, 1627, 1027, 814, 702, 630, 581, 543,
       514, 491, 471, 455, 441, 428, 418, 408, 399,
       391, 384, 378, 372, 366, 361, 356, 352, 347,
       343, 340, 336, 333, 330, 327, 324, 321, 318,
       316, 314, 311, 309, 307, 305, 303, 301, 299,
       298, 296, 294, 293, 291, 290, 288, 287, 285,
       284, 283, 281, 280, 279, 278, 277, 276, 275,
       273, 272
   };
/* *INDENT-ON* */

   DOR(mp_init(&a));

   /* number to result in a different size for every base: 67^(4 * 67) */
   mp_set(&a, 67);
   DO(mp_expt_n(&a, 268, &a));

   for (radix = 2; radix < 65; radix++) {
      DO(mp_radix_size(&a, radix, &size));
      EXPECT(size == results[radix]);
      a.sign = MP_NEG;
      DO(mp_radix_size(&a, radix, &size));
      EXPECT(size == (results[radix] + 1));
      a.sign = MP_ZPOS;
   }

   mp_clear(&a);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear(&a);
   return EXIT_FAILURE;
}

#define PRINTERR_V(...)

/* Some larger values to test the fast division algorithm */
static int test_s_mp_div_recursive(void)
{
   mp_int a, b, c_q, c_r, d_q, d_r;
   int size;

   DOR(mp_init_multi(&a, &b, &c_q, &c_r, &d_q, &d_r, NULL));

   for (size = MP_MUL_KARATSUBA_CUTOFF; size < (3 * MP_MUL_KARATSUBA_CUTOFF); size += 10) {
      printf("\rsizes = %d / %d", 10 * size, size);
      /* Relation 10:1 */
      DO(mp_rand(&a, 10 * size));
      DO(mp_rand(&b, size));
      DO(s_mp_div_recursive(&a, &b, &c_q, &c_r));
      DO(s_mp_div_school(&a, &b, &d_q, &d_r));
      EXPECT(mp_cmp(&c_q, &d_q) == MP_EQ);
      EXPECT(mp_cmp(&c_r, &d_r) == MP_EQ);
      printf("\rsizes = %d / %d", 2 * size, size);

      /* Relation 10:1 negative numerator*/
      DO(mp_rand(&a, 10 * size));
      DO(mp_neg(&a, &a));
      DO(mp_rand(&b, size));
      DO(s_mp_div_recursive(&a, &b, &c_q, &c_r));
      DO(s_mp_div_school(&a, &b, &d_q, &d_r));
      EXPECT(mp_cmp(&c_q, &d_q) == MP_EQ);
      EXPECT(mp_cmp(&c_r, &d_r) == MP_EQ);
      printf("\rsizes = %d / %d, negative numerator", 2 * size, size);

      /* Relation 10:1 negative denominator*/
      DO(mp_rand(&a, 10 * size));
      DO(mp_rand(&b, size));
      DO(mp_neg(&b, &b));
      DO(s_mp_div_recursive(&a, &b, &c_q, &c_r));
      DO(s_mp_div_school(&a, &b, &d_q, &d_r));
      EXPECT(mp_cmp(&c_q, &d_q) == MP_EQ);
      EXPECT(mp_cmp(&c_r, &d_r) == MP_EQ);
      printf("\rsizes = %d / %d, negative denominator", 2 * size, size);

      /* Relation 2:1 */
      DO(mp_rand(&a, 2 * size));
      DO(mp_rand(&b, size));
      DO(s_mp_div_recursive(&a, &b, &c_q, &c_r));
      DO(s_mp_div_school(&a, &b, &d_q, &d_r));
      EXPECT(mp_cmp(&c_q, &d_q) == MP_EQ);
      EXPECT(mp_cmp(&c_r, &d_r) == MP_EQ);
      printf("\rsizes = %d / %d", 3 * size, 2 * size);
      /* Upper limit 3:2 */
      DO(mp_rand(&a, 3 * size));
      DO(mp_rand(&b, 2 * size));
      DO(s_mp_div_recursive(&a, &b, &c_q, &c_r));
      DO(s_mp_div_school(&a, &b, &d_q, &d_r));
      EXPECT(mp_cmp(&c_q, &d_q) == MP_EQ);
      EXPECT(mp_cmp(&c_r, &d_r) == MP_EQ);
   }

   mp_clear_multi(&a, &b, &c_q, &c_r, &d_q, &d_r, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c_q, &c_r, &d_q, &d_r, NULL);
   return EXIT_FAILURE;
}

static int test_s_mp_div_small(void)
{
   mp_int a, b, c_q, c_r, d_q, d_r;
   int size;

   DOR(mp_init_multi(&a, &b, &c_q, &c_r, &d_q, &d_r, NULL));
   for (size = 1; size < MP_MUL_KARATSUBA_CUTOFF; size += 10) {
      printf("\rsizes = %d / %d", 2 * size, size);
      /* Relation 10:1 */
      DO(mp_rand(&a, 2 * size));
      DO(mp_rand(&b, size));
      DO(s_mp_div_small(&a, &b, &c_q, &c_r));
      DO(s_mp_div_school(&a, &b, &d_q, &d_r));
      EXPECT(mp_cmp(&c_q, &d_q) == MP_EQ);
      EXPECT(mp_cmp(&c_r, &d_r) == MP_EQ);
   }
   mp_clear_multi(&a, &b, &c_q, &c_r, &d_q, &d_r, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear_multi(&a, &b, &c_q, &c_r, &d_q, &d_r, NULL);
   return EXIT_FAILURE;
}


static int test_s_mp_radix_size_overestimate(void)
{

   mp_int a;
   int radix, n;
   size_t size, size2;
/* *INDENT-OFF* */
   size_t results[65] = {
       0u,  0u,  1627u, 1027u, 814u, 702u, 630u, 581u, 543u,
       514u, 491u, 471u, 455u, 441u, 428u, 418u, 408u, 399u,
       391u, 384u, 378u, 372u, 366u, 361u, 356u, 352u, 347u,
       343u, 340u, 336u, 333u, 330u, 327u, 324u, 321u, 318u,
       316u, 314u, 311u, 309u, 307u, 305u, 303u, 301u, 299u,
       298u, 296u, 294u, 293u, 291u, 290u, 288u, 287u, 285u,
       284u, 283u, 281u, 280u, 279u, 278u, 277u, 276u, 275u,
       273u, 272u
   };
/* *INDENT-ON* */

   DO(mp_init(&a));

   /* number to result in a different size for every base: 67^(4 * 67) */
   mp_set(&a, 67);
   DO(mp_expt_n(&a, 268, &a));

   for (radix = 2; radix < 65; radix++) {
      DO(s_mp_radix_size_overestimate(&a, radix, &size));
      EXPECT(size >= results[radix]);
      EXPECT(size < results[radix] + 20); /* some error bound */
      a.sign = MP_NEG;
      DO(s_mp_radix_size_overestimate(&a, radix, &size));
      EXPECT(size >= results[radix]);
      EXPECT(size < results[radix] + 20); /* some error bound */
      a.sign = MP_ZPOS;
   }

   /* randomized test */
   for (n = 1; n < 1024; n += 1234) {
      DO(mp_rand(&a, n));

      for (radix = 2; radix < 65; radix++) {
         DO(s_mp_radix_size_overestimate(&a, radix, &size));
         DO(mp_radix_size(&a, radix, &size2));
         EXPECT(size >= size2);
         EXPECT(size < size2 + 20); /* some error bound */
         a.sign = MP_NEG;
         DO(s_mp_radix_size_overestimate(&a, radix, &size));
         DO(mp_radix_size(&a, radix, &size2));
         EXPECT(size >= size2);
         EXPECT(size < size2 + 20); /* some error bound */
         a.sign = MP_ZPOS;
      }
   }

   mp_clear(&a);
   return EXIT_SUCCESS;
LBL_ERR:
   mp_clear(&a);
   return EXIT_FAILURE;
}


static int test_mp_read_write_ubin(void)
{
   mp_int a, b, c;
   size_t size, len;
   uint8_t *buf = NULL;

   DOR(mp_init_multi(&a, &b, &c, NULL));

   DO(mp_rand(&a, 15));
   DO(mp_neg(&a, &b));

   size = mp_ubin_size(&a);
   printf("mp_to_ubin_size  %zu - ", size);
   buf = (uint8_t *)malloc(sizeof(*buf) * size);
   if (buf == NULL) {
      fprintf(stderr, "test_read_write_binaries (u) failed to allocate %zu bytes\n",
              sizeof(*buf) * size);
      goto LBL_ERR;
   }

   DO(mp_to_ubin(&a, buf, size, &len));
   printf("mp_to_ubin len = %zu", len);

   DO(mp_from_ubin(&c, buf, len));

   if (mp_cmp(&a, &c) != MP_EQ) {
      fprintf(stderr, "to/from ubin cycle failed\n");
      goto LBL_ERR;
   }
   free(buf);
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   free(buf);
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_read_write_sbin(void)
{
   mp_int a, b, c;
   size_t size, len;
   uint8_t *buf = NULL;

   DOR(mp_init_multi(&a, &b, &c, NULL));

   DO(mp_rand(&a, 15));
   DO(mp_neg(&a, &b));

   size = mp_sbin_size(&a);
   printf("mp_to_sbin_size  %zu - ", size);
   buf = (uint8_t *)malloc(sizeof(*buf) * size);
   if (buf == NULL) {
      fprintf(stderr, "test_read_write_binaries (s) failed to allocate %zu bytes\n",
              sizeof(*buf) * size);
      goto LBL_ERR;
   }

   DO(mp_to_sbin(&b, buf, size, &len));
   printf("mp_to_sbin len = %zu", len);

   DO(mp_from_sbin(&c, buf, len));

   if (mp_cmp(&b, &c) != MP_EQ) {
      fprintf(stderr, "to/from ubin cycle failed\n");
      goto LBL_ERR;
   }

   free(buf);
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   free(buf);
   mp_clear_multi(&a, &b, &c, NULL);
   return EXIT_FAILURE;
}

static int test_mp_pack_unpack(void)
{
   mp_int a, b;
   size_t written, count;
   uint8_t *buf = NULL;

   mp_order order = MP_LSB_FIRST;
   mp_endian endianess = MP_NATIVE_ENDIAN;

   DOR(mp_init_multi(&a, &b, NULL));
   DO(mp_rand(&a, 15));

   count = mp_pack_count(&a, 0uL, 1uL);

   buf = (uint8_t *)malloc(count);
   if (buf == NULL) {
      fprintf(stderr, "test_pack_unpack failed to allocate\n");
      goto LBL_ERR;
   }

   DO(mp_pack((void *)buf, count, &written, order, 1uL,
              endianess, 0uL, &a));
   DO(mp_unpack(&b, count, order, 1uL,
                endianess, 0uL, (const void *)buf));

   if (mp_cmp(&a, &b) != MP_EQ) {
      fprintf(stderr, "pack/unpack cycle failed\n");
      goto LBL_ERR;
   }

   free(buf);
   mp_clear_multi(&a, &b, NULL);
   return EXIT_SUCCESS;
LBL_ERR:
   free(buf);
   mp_clear_multi(&a, &b, NULL);
   return EXIT_FAILURE;
}

#ifndef LTM_TEST_DYNAMIC
#define ONLY_PUBLIC_API_C
#endif

static int unit_tests(int argc, char **argv)
{
   static const struct {
      const char *name;
      int (*fn)(void);
   } test[] = {
#define T0(n)              { #n, test_##n }
#define T1(n, o)           { #n, MP_HAS(o) ? test_##n : NULL }
#define T2(n, o1, o2)      { #n, (MP_HAS(o1) && MP_HAS(o2)) ? test_##n : NULL }
#define T3(n, o1, o2, o3)  { #n, (MP_HAS(o1) && MP_HAS(o2) && MP_HAS(o3)) ? test_##n : NULL }
      T0(feature_detection),
      T0(trivial_stuff),
      T2(mp_get_set_i32, MP_GET_I32, MP_GET_MAG_U32),
      T2(mp_get_set_i64, MP_GET_I64, MP_GET_MAG_U64),
      T1(mp_and, MP_AND),
      T1(mp_cnt_lsb, MP_CNT_LSB),
      T1(mp_complement, MP_COMPLEMENT),
      T1(mp_decr, MP_SUB_D),
      T2(s_mp_div_3, ONLY_PUBLIC_API, S_MP_DIV_3),
      T1(mp_dr_reduce, MP_DR_REDUCE),
      T2(mp_pack_unpack,MP_PACK, MP_UNPACK),
      T2(mp_fread_fwrite, MP_FREAD, MP_FWRITE),
      T1(mp_get_u32, MP_GET_I32),
      T1(mp_get_u64, MP_GET_I64),
      T1(mp_get_ul, MP_GET_L),
      T1(mp_log_n, MP_LOG_N),
      T1(mp_incr, MP_ADD_D),
      T1(mp_invmod, MP_INVMOD),
      T1(mp_is_square, MP_IS_SQUARE),
      T1(mp_kronecker, MP_KRONECKER),
      T1(mp_montgomery_reduce, MP_MONTGOMERY_REDUCE),
      T1(mp_root_n, MP_ROOT_N),
      T1(mp_or, MP_OR),
      T1(mp_prime_is_prime, MP_PRIME_IS_PRIME),
      T1(mp_prime_next_prime, MP_PRIME_NEXT_PRIME),
      T1(mp_prime_rand, MP_PRIME_RAND),
      T1(mp_rand, MP_RAND),
      T1(mp_read_radix, MP_READ_RADIX),
      T1(mp_read_write_ubin, MP_TO_UBIN),
      T1(mp_read_write_sbin, MP_TO_SBIN),
      T1(mp_reduce_2k, MP_REDUCE_2K),
      T1(mp_reduce_2k_l, MP_REDUCE_2K_L),
      T1(mp_radix_size, MP_RADIX_SIZE),
      T2(s_mp_radix_size_overestimate, ONLY_PUBLIC_API, S_MP_RADIX_SIZE_OVERESTIMATE),
#if defined(MP_HAS_SET_DOUBLE)
      T1(mp_set_double, MP_SET_DOUBLE),
#endif
      T1(mp_signed_rsh, MP_SIGNED_RSH),
      T2(mp_sqrt, MP_SQRT, MP_ROOT_N),
      T1(mp_sqrtmod_prime, MP_SQRTMOD_PRIME),
      T1(mp_xor, MP_XOR),
      T3(s_mp_div_recursive, ONLY_PUBLIC_API, S_MP_DIV_RECURSIVE, S_MP_DIV_SCHOOL),
      T3(s_mp_div_small, ONLY_PUBLIC_API, S_MP_DIV_SMALL, S_MP_DIV_SCHOOL),
      T2(s_mp_mul_balance, ONLY_PUBLIC_API, S_MP_MUL_BALANCE),
      T2(s_mp_mul_karatsuba, ONLY_PUBLIC_API, S_MP_MUL_KARATSUBA),
      T2(s_mp_sqr_karatsuba, ONLY_PUBLIC_API, S_MP_SQR_KARATSUBA),
      T2(s_mp_mul_toom, ONLY_PUBLIC_API, S_MP_MUL_TOOM),
      T2(s_mp_sqr_toom, ONLY_PUBLIC_API, S_MP_SQR_TOOM)
#undef T3
#undef T2
#undef T1
   };
   unsigned long i, ok, fail, nop;
   uint64_t t;
   int j;

   ok = fail = nop = 0;

   t = (uint64_t)time(NULL);
   printf("SEED: 0x%" PRIx64 "\n\n", t);
   s_mp_rand_jenkins_init(t);
   mp_rand_source(s_mp_rand_jenkins);

   for (i = 0; i < (sizeof(test) / sizeof(test[0])); ++i) {
      if (argc > 1) {
         for (j = 1; j < argc; ++j) {
            if (strstr(test[i].name, argv[j]) != NULL) {
               break;
            }
         }
         if (j == argc) continue;
      }
      printf("TEST %s\n", test[i].name);
      if (test[i].fn == NULL) {
         nop++;
         printf("NOP %s\n\n", test[i].name);
      } else if (test[i].fn() == EXIT_SUCCESS) {
         ok++;
         printf("\n");
      } else {
         fail++;
         printf("\n\nFAIL %s\n\n", test[i].name);
      }
   }
   fprintf(fail?stderr:stdout, "Tests OK/NOP/FAIL: %lu/%lu/%lu\n", ok, nop, fail);

   if (fail != 0) return EXIT_FAILURE;
   else return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
   print_header();

   return unit_tests(argc, argv);
}
