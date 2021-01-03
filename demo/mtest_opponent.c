#include "shared.h"

#ifdef LTM_MTEST_REAL_RAND
#define LTM_MTEST_RAND_SEED  time(NULL)
#else
#define LTM_MTEST_RAND_SEED  23
#endif

#define DRAW(a) do{ ndraw(&(a), #a); }while(0)

/*
   Get tokens. It is just a very(!) simple fgets(3) that does not keep line endings.

   Implementation follows along "man 3 fgets", some of which is quoted.
 */
static int s_mp_get_token(char *s, int size, FILE *stream)
{
   char *s_bar = s;
   int c;
   bool eol_hit = false;

   /* "fgets [...] reads in at most one less than size characters from stream"  */
   while (--size)  {
      /* "Reading stops after an EOF or a newline." We stop only for EOF here */
      if ((c = fgetc(stream)) == EOF) {
         /* "Returns [...] NULL on error or when end of file occurs while no characters have been read" */
         if ((s_bar == s) || (ferror(stream) != 0)) {
            return -1;
         }
         break;
      }
      /* Ignore line-breaks but keep reading to get them out of the stream-buffer */
      if ((c == '\n') || (c == '\r')) {
         eol_hit = true;
         continue;
      }
      /* Stop reading after linebreak */
      if (eol_hit) {
         /* We already read the character after the linebreak, put it back */
         ungetc(c, stream);
         break;
      }
      *s_bar++ = c;
   }
   if (size == 0) return -2;
   /* "A terminating null byte ('\0') is stored after the last character in the buffer" */
   *s_bar = '\0';
   return 0;
}

#define TMP_(r,l) r ## _ ## l
#define TMP(r,l) TMP_(r,l)

#define GET_TOKEN(str, stream) \
   do { \
      int TMP(ret,__LINE__) = s_mp_get_token((str), sizeof(str), (stream)); \
      if (TMP(ret,__LINE__) < 0) { fprintf(stderr, "\n%d: s_mp_get_token failed with error %d\n", __LINE__, TMP(ret,__LINE__)); goto LBL_ERR; } \
   } while(0)

static int mtest_opponent(void)
{
   char cmd[4096];
   char buf[4096];
   int ix;
   unsigned rr;
   mp_int a, b, c, d, e, f;
   unsigned long expt_n, add_n, sub_n, mul_n, div_n, sqr_n, mul2d_n, div2d_n,
            gcd_n, lcm_n, inv_n, div2_n, mul2_n, add_d_n, sub_d_n;

   srand(LTM_MTEST_RAND_SEED);

   if (mp_init_multi(&a, &b, &c, &d, &e, &f, NULL)!= MP_OKAY)
      return EXIT_FAILURE;

   div2_n = mul2_n = inv_n = expt_n = lcm_n = gcd_n = add_n =
                                         sub_n = mul_n = div_n = sqr_n = mul2d_n = div2d_n = add_d_n = sub_d_n = 0;

#ifndef MP_FIXED_CUTOFFS
   /* force KARA and TOOM to enable despite cutoffs */
   MP_SQR_KARATSUBA_CUTOFF = MP_MUL_KARATSUBA_CUTOFF = 8;
   MP_SQR_TOOM_CUTOFF = MP_MUL_TOOM_CUTOFF = 16;
#endif

   for (;;) {
      /* randomly clear and re-init one variable, this has the affect of triming the alloc space */
      switch (abs(rand()) % 7) {
      case 0:
         mp_clear(&a);
         DO(mp_init(&a));
         break;
      case 1:
         mp_clear(&b);
         DO(mp_init(&b));
         break;
      case 2:
         mp_clear(&c);
         DO(mp_init(&c));
         break;
      case 3:
         mp_clear(&d);
         DO(mp_init(&d));
         break;
      case 4:
         mp_clear(&e);
         DO(mp_init(&e));
         break;
      case 5:
         mp_clear(&f);
         DO(mp_init(&f));
         break;
      case 6:
         break;        /* don't clear any */
      }


      printf("%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu ",
             add_n, sub_n, mul_n, div_n, sqr_n, mul2d_n, div2d_n, gcd_n, lcm_n,
             expt_n, inv_n, div2_n, mul2_n, add_d_n, sub_d_n);
      GET_TOKEN(cmd, stdin);
      printf("%-6s ]\r", cmd);
      fflush(stdout);
      if (strcmp(cmd, "mul2d") == 0) {
         ++mul2d_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         sscanf(buf, "%u", &rr);
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));

         DO(mp_mul_2d(&a, (int)rr, &a));
         a.sign = b.sign;
         if (mp_cmp(&a, &b) != MP_EQ) {
            printf("mul2d failed, rr == %u\n", rr);
            DRAW(a);
            DRAW(b);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "div2d") == 0) {
         ++div2d_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         sscanf(buf, "%u", &rr);
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));

         DO(mp_div_2d(&a, (int)rr, &a, &e));
         a.sign = b.sign;
         if ((a.used == b.used) && (a.used == 0)) {
            a.sign = b.sign = MP_ZPOS;
         }
         if (mp_cmp(&a, &b) != MP_EQ) {
            printf("div2d failed, rr == %u\n", rr);
            DRAW(a);
            DRAW(b);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "add") == 0) {
         ++add_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&c, buf, 64));
         DO(mp_copy(&a, &d));
         DO(mp_add(&d, &b, &d));
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("add %lu failure!\n", add_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            DRAW(d);
            goto LBL_ERR;
         }

         /* test the sign/unsigned storage functions */

         rr = (unsigned)mp_sbin_size(&c);
         DO(mp_to_sbin(&c, (uint8_t *) cmd, (size_t)rr, NULL));
         memset(cmd + rr, rand() & 0xFF, sizeof(cmd) - rr);
         DO(mp_from_sbin(&d, (uint8_t *) cmd, (size_t)rr));
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("mp_signed_bin failure!\n");
            DRAW(c);
            DRAW(d);
            goto LBL_ERR;
         }

         rr = (unsigned)mp_ubin_size(&c);
         DO(mp_to_ubin(&c, (uint8_t *) cmd, (size_t)rr, NULL));
         memset(cmd + rr, rand() & 0xFF, sizeof(cmd) - rr);
         DO(mp_from_ubin(&d, (uint8_t *) cmd, (size_t)rr));
         if (mp_cmp_mag(&c, &d) != MP_EQ) {
            printf("mp_unsigned_bin failure!\n");
            DRAW(c);
            DRAW(d);
            goto LBL_ERR;
         }

      } else if (strcmp(cmd, "sub") == 0) {
         ++sub_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&c, buf, 64));
         DO(mp_copy(&a, &d));
         DO(mp_sub(&d, &b, &d));
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("sub %lu failure!\n", sub_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            DRAW(d);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "mul") == 0) {
         ++mul_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&c, buf, 64));
         DO(mp_copy(&a, &d));
         DO(mp_mul(&d, &b, &d));
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("mul %lu failure!\n", mul_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            DRAW(d);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "div") == 0) {
         ++div_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&c, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&d, buf, 64));

         DO(mp_div(&a, &b, &e, &f));
         if ((mp_cmp(&c, &e) != MP_EQ) || (mp_cmp(&d, &f) != MP_EQ)) {
            printf("div %lu %d, %d, failure!\n", div_n, mp_cmp(&c, &e),
                   mp_cmp(&d, &f));
            DRAW(a);
            DRAW(b);
            DRAW(c);
            DRAW(d);
            DRAW(e);
            DRAW(f);
            goto LBL_ERR;
         }

      } else if (strcmp(cmd, "sqr") == 0) {
         ++sqr_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         DO(mp_copy(&a, &c));
         DO(mp_sqr(&c, &c));
         if (mp_cmp(&b, &c) != MP_EQ) {
            printf("sqr %lu failure!\n", sqr_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "gcd") == 0) {
         ++gcd_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&c, buf, 64));
         DO(mp_copy(&a, &d));
         DO(mp_gcd(&d, &b, &d));
         d.sign = c.sign;
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("gcd %lu failure!\n", gcd_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            DRAW(d);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "lcm") == 0) {
         ++lcm_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&c, buf, 64));
         DO(mp_copy(&a, &d));
         DO(mp_lcm(&d, &b, &d));
         d.sign = c.sign;
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("lcm %lu failure!\n", lcm_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            DRAW(d);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "expt") == 0) {
         ++expt_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&c, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&d, buf, 64));
         DO(mp_copy(&a, &e));
         DO(mp_exptmod(&e, &b, &c, &e));
         if (mp_cmp(&d, &e) != MP_EQ) {
            printf("expt %lu failure!\n", expt_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            DRAW(d);
            DRAW(e);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "invmod") == 0) {
         ++inv_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&c, buf, 64));
         DO(mp_invmod(&a, &b, &d));
         DO(mp_mulmod(&d, &a, &b, &e));
         if (mp_cmp_d(&e, 1u) != MP_EQ) {
            printf("inv [wrong value from MPI?!] failure\n");
            DRAW(a);
            DRAW(b);
            DRAW(c);
            DRAW(d);
            DRAW(e);
            DO(mp_gcd(&a, &b, &e));
            DRAW(e);
            goto LBL_ERR;
         }

      } else if (strcmp(cmd, "div2") == 0) {
         ++div2_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         DO(mp_div_2(&a, &c));
         if (mp_cmp(&c, &b) != MP_EQ) {
            printf("div_2 %lu failure\n", div2_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "mul2") == 0) {
         ++mul2_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         DO(mp_mul_2(&a, &c));
         if (mp_cmp(&c, &b) != MP_EQ) {
            printf("mul_2 %lu failure\n", mul2_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "add_d") == 0) {
         ++add_d_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         sscanf(buf, "%d", &ix);
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         DO(mp_add_d(&a, (mp_digit)ix, &c));
         if (mp_cmp(&b, &c) != MP_EQ) {
            printf("add_d %lu failure\n", add_d_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            printf("d == %d\n", ix);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "sub_d") == 0) {
         ++sub_d_n;
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&a, buf, 64));
         GET_TOKEN(buf, stdin);
         sscanf(buf, "%d", &ix);
         GET_TOKEN(buf, stdin);
         DO(mp_read_radix(&b, buf, 64));
         DO(mp_sub_d(&a, (mp_digit)ix, &c));
         if (mp_cmp(&b, &c) != MP_EQ) {
            printf("sub_d %lu failure\n", sub_d_n);
            DRAW(a);
            DRAW(b);
            DRAW(c);
            printf("d == %d\n", ix);
            goto LBL_ERR;
         }
      } else if (strcmp(cmd, "exit") == 0) {
         printf("\nokay, exiting now\n");
         break;
      }
   }

   mp_clear_multi(&a, &b, &c, &d, &e, &f, NULL);
   printf("\n");
   return 0;

LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, &e, &f, NULL);
   printf("\n");
   return EXIT_FAILURE;
}

int main(void)
{
   print_header();

   return mtest_opponent();
}
