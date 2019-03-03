#include "shared.h"

static void ndraw(mp_int *a, const char *name)
{
   char buf[16000];

   printf("%s: ", name);
   mp_toradix(a, buf, 10);
   printf("%s\n", buf);
   mp_toradix(a, buf, 16);
   printf("0x%s\n", buf);
}

static void draw(mp_int *a)
{
   ndraw(a, "");
}

static void _panic(int l)
{
   fprintf(stderr, "\n%d: fgets failed\n", l);
   exit(EXIT_FAILURE);
}

#define FGETS(str, size, stream) \
   { \
      char *ret = fgets(str, size, stream); \
      if (!ret) { _panic(__LINE__); } \
   }

static mp_int a, b, c, d, e, f;

static void _cleanup(void)
{
   mp_clear_multi(&a, &b, &c, &d, &e, &f, NULL);
   printf("\n");
}

int mtest_opponent(void)
{
   char cmd[4096];
   char buf[4096];
   int ix;
   unsigned rr;
   unsigned long expt_n, add_n, sub_n, mul_n, div_n, sqr_n, mul2d_n, div2d_n,
            gcd_n, lcm_n, inv_n, div2_n, mul2_n, add_d_n, sub_d_n;

   if (mp_init_multi(&a, &b, &c, &d, &e, &f, NULL)!= MP_OKAY)
      return EXIT_FAILURE;

   atexit(_cleanup);

   div2_n = mul2_n = inv_n = expt_n = lcm_n = gcd_n = add_n =
                                         sub_n = mul_n = div_n = sqr_n = mul2d_n = div2d_n = add_d_n = sub_d_n = 0;

   /* force KARA and TOOM to enable despite cutoffs */
   KARATSUBA_SQR_CUTOFF = KARATSUBA_MUL_CUTOFF = 8;
   TOOM_SQR_CUTOFF = TOOM_MUL_CUTOFF = 16;

   for (;;) {
      /* randomly clear and re-init one variable, this has the affect of triming the alloc space */
      switch (abs(rand()) % 7) {
      case 0:
         mp_clear(&a);
         mp_init(&a);
         break;
      case 1:
         mp_clear(&b);
         mp_init(&b);
         break;
      case 2:
         mp_clear(&c);
         mp_init(&c);
         break;
      case 3:
         mp_clear(&d);
         mp_init(&d);
         break;
      case 4:
         mp_clear(&e);
         mp_init(&e);
         break;
      case 5:
         mp_clear(&f);
         mp_init(&f);
         break;
      case 6:
         break;        /* don't clear any */
      }


      printf("%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu/%4lu ",
             add_n, sub_n, mul_n, div_n, sqr_n, mul2d_n, div2d_n, gcd_n, lcm_n,
             expt_n, inv_n, div2_n, mul2_n, add_d_n, sub_d_n);
      FGETS(cmd, 4095, stdin);
      cmd[strlen(cmd) - 1u] = '\0';
      printf("%-6s ]\r", cmd);
      fflush(stdout);
      if (strcmp(cmd, "mul2d") == 0) {
         ++mul2d_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         sscanf(buf, "%u", &rr);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);

         mp_mul_2d(&a, rr, &a);
         a.sign = b.sign;
         if (mp_cmp(&a, &b) != MP_EQ) {
            printf("mul2d failed, rr == %u\n", rr);
            draw(&a);
            draw(&b);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "div2d") == 0) {
         ++div2d_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         sscanf(buf, "%u", &rr);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);

         mp_div_2d(&a, rr, &a, &e);
         a.sign = b.sign;
         if ((a.used == b.used) && (a.used == 0)) {
            a.sign = b.sign = MP_ZPOS;
         }
         if (mp_cmp(&a, &b) != MP_EQ) {
            printf("div2d failed, rr == %u\n", rr);
            draw(&a);
            draw(&b);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "add") == 0) {
         ++add_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&c, buf, 64);
         mp_copy(&a, &d);
         mp_add(&d, &b, &d);
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("add %lu failure!\n", add_n);
            draw(&a);
            draw(&b);
            draw(&c);
            draw(&d);
            return EXIT_FAILURE;
         }

         /* test the sign/unsigned storage functions */

         rr = mp_signed_bin_size(&c);
         mp_to_signed_bin(&c, (unsigned char *) cmd);
         memset(cmd + rr, rand() & 0xFFu, sizeof(cmd) - rr);
         mp_read_signed_bin(&d, (unsigned char *) cmd, rr);
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("mp_signed_bin failure!\n");
            draw(&c);
            draw(&d);
            return EXIT_FAILURE;
         }


         rr = mp_unsigned_bin_size(&c);
         mp_to_unsigned_bin(&c, (unsigned char *) cmd);
         memset(cmd + rr, rand() & 0xFFu, sizeof(cmd) - rr);
         mp_read_unsigned_bin(&d, (unsigned char *) cmd, rr);
         if (mp_cmp_mag(&c, &d) != MP_EQ) {
            printf("mp_unsigned_bin failure!\n");
            draw(&c);
            draw(&d);
            return EXIT_FAILURE;
         }

      } else if (strcmp(cmd, "sub") == 0) {
         ++sub_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&c, buf, 64);
         mp_copy(&a, &d);
         mp_sub(&d, &b, &d);
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("sub %lu failure!\n", sub_n);
            draw(&a);
            draw(&b);
            draw(&c);
            draw(&d);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "mul") == 0) {
         ++mul_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&c, buf, 64);
         mp_copy(&a, &d);
         mp_mul(&d, &b, &d);
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("mul %lu failure!\n", mul_n);
            draw(&a);
            draw(&b);
            draw(&c);
            draw(&d);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "div") == 0) {
         ++div_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&c, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&d, buf, 64);

         mp_div(&a, &b, &e, &f);
         if ((mp_cmp(&c, &e) != MP_EQ) || (mp_cmp(&d, &f) != MP_EQ)) {
            printf("div %lu %d, %d, failure!\n", div_n, mp_cmp(&c, &e),
                   mp_cmp(&d, &f));
            draw(&a);
            draw(&b);
            draw(&c);
            draw(&d);
            draw(&e);
            draw(&f);
            return EXIT_FAILURE;
         }

      } else if (strcmp(cmd, "sqr") == 0) {
         ++sqr_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         mp_copy(&a, &c);
         mp_sqr(&c, &c);
         if (mp_cmp(&b, &c) != MP_EQ) {
            printf("sqr %lu failure!\n", sqr_n);
            draw(&a);
            draw(&b);
            draw(&c);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "gcd") == 0) {
         ++gcd_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&c, buf, 64);
         mp_copy(&a, &d);
         mp_gcd(&d, &b, &d);
         d.sign = c.sign;
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("gcd %lu failure!\n", gcd_n);
            draw(&a);
            draw(&b);
            draw(&c);
            draw(&d);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "lcm") == 0) {
         ++lcm_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&c, buf, 64);
         mp_copy(&a, &d);
         mp_lcm(&d, &b, &d);
         d.sign = c.sign;
         if (mp_cmp(&c, &d) != MP_EQ) {
            printf("lcm %lu failure!\n", lcm_n);
            draw(&a);
            draw(&b);
            draw(&c);
            draw(&d);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "expt") == 0) {
         ++expt_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&c, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&d, buf, 64);
         mp_copy(&a, &e);
         mp_exptmod(&e, &b, &c, &e);
         if (mp_cmp(&d, &e) != MP_EQ) {
            printf("expt %lu failure!\n", expt_n);
            draw(&a);
            draw(&b);
            draw(&c);
            draw(&d);
            draw(&e);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "invmod") == 0) {
         ++inv_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&c, buf, 64);
         mp_invmod(&a, &b, &d);
         mp_mulmod(&d, &a, &b, &e);
         if (mp_cmp_d(&e, 1uL) != MP_EQ) {
            printf("inv [wrong value from MPI?!] failure\n");
            draw(&a);
            draw(&b);
            draw(&c);
            draw(&d);
            draw(&e);
            mp_gcd(&a, &b, &e);
            draw(&e);
            return EXIT_FAILURE;
         }

      } else if (strcmp(cmd, "div2") == 0) {
         ++div2_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         mp_div_2(&a, &c);
         if (mp_cmp(&c, &b) != MP_EQ) {
            printf("div_2 %lu failure\n", div2_n);
            draw(&a);
            draw(&b);
            draw(&c);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "mul2") == 0) {
         ++mul2_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         mp_mul_2(&a, &c);
         if (mp_cmp(&c, &b) != MP_EQ) {
            printf("mul_2 %lu failure\n", mul2_n);
            draw(&a);
            draw(&b);
            draw(&c);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "add_d") == 0) {
         ++add_d_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         sscanf(buf, "%d", &ix);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         mp_add_d(&a, ix, &c);
         if (mp_cmp(&b, &c) != MP_EQ) {
            printf("add_d %lu failure\n", add_d_n);
            draw(&a);
            draw(&b);
            draw(&c);
            printf("d == %d\n", ix);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "sub_d") == 0) {
         ++sub_d_n;
         FGETS(buf, 4095, stdin);
         mp_read_radix(&a, buf, 64);
         FGETS(buf, 4095, stdin);
         sscanf(buf, "%d", &ix);
         FGETS(buf, 4095, stdin);
         mp_read_radix(&b, buf, 64);
         mp_sub_d(&a, ix, &c);
         if (mp_cmp(&b, &c) != MP_EQ) {
            printf("sub_d %lu failure\n", sub_d_n);
            draw(&a);
            draw(&b);
            draw(&c);
            printf("d == %d\n", ix);
            return EXIT_FAILURE;
         }
      } else if (strcmp(cmd, "exit") == 0) {
         printf("\nokay, exiting now\n");
         break;
      }
   }

   return 0;
}
