/* Tune the Karatsuba parameters
 *
 * Tom St Denis, tstdenis82@gmail.com
 */
#include "../tommath.h"
#include "../tommath_private.h"
#include <time.h>
#include <inttypes.h>
#include <errno.h>

/*
   Please take in mind that both multiplicands are of the same size. The balancing
   mechanism in mp_balance works well but has some overhead itself. You can test
   the behaviour of it with the option "-o" followed by a (small) positive number 'x'
   to generate ratios of the form 1:x.
*/

static uint64_t s_timer_function(void);
static void s_timer_start(void);
static uint64_t s_timer_stop(void);
static uint64_t s_time_mul(int size);
static uint64_t s_time_sqr(int size);
static void s_usage(char *s);

static uint64_t s_timer_function(void)
{
#if _POSIX_C_SOURCE >= 199309L
#define LTM_BILLION 1000000000
   struct timespec ts;

   /* TODO: Sets errno in case of error. Use? */
   clock_gettime(CLOCK_MONOTONIC, &ts);
   return (((uint64_t)ts.tv_sec) * LTM_BILLION + (uint64_t)ts.tv_nsec);
#else
   clock_t t;
   t = clock();
   if (t < (clock_t)(0)) {
      return (uint64_t)(0);
   }
   return (uint64_t)(t);
#endif
}

/* generic ISO C timer */
static uint64_t s_timer_tmp;
static void s_timer_start(void)
{
   s_timer_tmp = s_timer_function();
}
static uint64_t s_timer_stop(void)
{
   return s_timer_function() - s_timer_tmp;
}


static int s_check_result;
static int s_number_of_test_loops;
static int s_stabilization_extra;
static int s_offset = 1;

#define s_mp_mul(a, b, c) s_mp_mul_digs(a, b, c, (a)->used + (b)->used + 1)
static uint64_t s_time_mul(int size)
{
   int x, e;
   mp_int  a, b, c, d;
   uint64_t t1;

   if ((e = mp_init_multi(&a, &b, &c, &d, NULL)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   if ((e = mp_rand(&a, size * s_offset)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }
   if ((e = mp_rand(&b, size)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   s_timer_start();
   for (x = 0; x < s_number_of_test_loops; x++) {
      if ((e = mp_mul(&a,&b,&c)) != MP_OKAY) {
         t1 = UINT64_MAX;
         goto LTM_ERR;
      }
      if (s_check_result == 1) {
         if ((e = s_mp_mul(&a,&b,&d)) != MP_OKAY) {
            t1 = UINT64_MAX;
            goto LTM_ERR;
         }
         if (mp_cmp(&c, &d) != MP_EQ) {
            /* Time of 0 cannot happen (famous last words?) */
            t1 = 0uLL;
            goto LTM_ERR;
         }
      }
   }

   t1 = s_timer_stop();
LTM_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return t1;
}

static uint64_t s_time_sqr(int size)
{
   int x, e;
   mp_int  a, b, c;
   uint64_t t1;

   if ((e = mp_init_multi(&a, &b, &c, NULL)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   if ((e = mp_rand(&a, size)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   s_timer_start();
   for (x = 0; x < s_number_of_test_loops; x++) {
      if ((e = mp_sqr(&a,&b)) != MP_OKAY) {
         t1 = UINT64_MAX;
         goto LTM_ERR;
      }
      if (s_check_result == 1) {
         if ((e = s_mp_sqr(&a,&c)) != MP_OKAY) {
            t1 = UINT64_MAX;
            goto LTM_ERR;
         }
         if (mp_cmp(&c, &b) != MP_EQ) {
            t1 = 0uLL;
            goto LTM_ERR;
         }
      }
   }

   t1 = s_timer_stop();
LTM_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return t1;
}

static void s_usage(char *s)
{
   fprintf(stderr,"Usage: %s [TvcpGbtrSLFfMmosh]\n",s);
   fprintf(stderr,"          -T testmode, for use with testme.sh\n");
   fprintf(stderr,"          -v verbose, print all timings\n");
   fprintf(stderr,"          -c check results\n");
   fprintf(stderr,"          -p print benchmark of final cutoffs in files \"multiplying\"\n");
   fprintf(stderr,"             and \"squaring\"\n");
   fprintf(stderr,"          -G [string] suffix for the filenames listed above\n");
   fprintf(stderr,"             Implies '-p'\n");
   fprintf(stderr,"          -b print benchmark of bncore.c\n");
   fprintf(stderr,"          -t prints space (0x20) separated results\n");
   fprintf(stderr,"          -r [64] number of rounds\n");
   fprintf(stderr,"          -S [0xdeadbeef] seed for PRNG\n");
   fprintf(stderr,"          -L [3] number of negative values accumulated until the result is accepted\n");
   fprintf(stderr,"          -M [3000] upper limit of T-C tests/prints\n");
   fprintf(stderr,"          -m [1] increment of T-C tests/prints\n");
   fprintf(stderr,"          -o [1] multiplier for the second multiplicand\n");
   fprintf(stderr,"             (Not for computing the cut-offs!)\n");
   fprintf(stderr,"          -s 'preset' use values in 'preset' for printing.\n");
   fprintf(stderr,"             'preset' is a comma separated string with cut-offs for\n");
   fprintf(stderr,"             ksm, kss, tc3m, tc3s in that order\n");
   fprintf(stderr,"             ksm  = karatsuba multiplication\n");
   fprintf(stderr,"             kss  = karatsuba squaring\n");
   fprintf(stderr,"             tc3m = Toom-Cook 3-way multiplication\n");
   fprintf(stderr,"             tc3s = Toom-Cook 3-way squaring\n");
   fprintf(stderr,"             Implies '-p'\n");
   fprintf(stderr,"          -h this message\n");
}



int main(int argc, char **argv)
{
   uint64_t t1, t2;
   int x, i, j;
   int count = 0;

   int testmode = 0;
   int verbose = 0;
   int print = 0;
   int bncore = 0;
   int terse = 0;

   int upper_limit_print = 3000;
   int increment_print = 1;

   int printpreset = 0;
   /*int preset[8];*/
   int base = 10;
   char *endptr, *str;
   long val;

   uint64_t seed = 0xdeadbeef;

   int opt;
   int ksm, kss, tc3m, tc3s;

   FILE *squaring, *multiplying;
   char mullog[256] = "multiplying";
   char sqrlog[256] = "squaring";
   s_number_of_test_loops = 64;
   s_stabilization_extra = 3;

   /* Very simple option parser, please treat it nicely. */
   if (argc != 1) {
      for (opt = 1; (opt < argc) && (argv[opt][0] == '-'); opt++) {
         switch (argv[opt][1]) {
         case 'T':
            testmode = 1;
            s_check_result = 1;
            upper_limit_print = 1000;
            increment_print = 11;
            s_number_of_test_loops = 1;
            s_stabilization_extra = 1;
            s_offset = 1;
            break;
         case 'v':
            verbose = 1;
            break;
         case 'c':
            s_check_result = 1;
            break;
         case 'p':
            print = 1;
            break;
         case 'G':
            print = 1;
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            /* manual strcat() */
            for (i = 0; i < 255; i++) {
               if (mullog[i] == '\0') {
                  break;
               }
            }
            for (j = 0; i < 255; j++, i++) {
               mullog[i] = argv[opt][j];
               if (argv[opt][j] == '\0') {
                  break;
               }
            }
            for (i = 0; i < 255; i++) {
               if (sqrlog[i] == '\0') {
                  break;
               }
            }
            for (j = 0; i < 255; j++, i++) {
               sqrlog[i] = argv[opt][j];
               if (argv[opt][j] == '\0') {
                  break;
               }
            }
            break;
         case 'b':
            bncore = 1;
            break;
         case 't':
            terse = 1;
            break;
         case 'S':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            str = argv[opt];
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                || (errno != 0 && val == 0)) {
               fprintf(stderr,"Seed %s not usable\n", argv[opt]);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No seed given?\n");
               exit(EXIT_FAILURE);
            }
            seed = (uint64_t)val;
            break;
         case 'L':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            str = argv[opt];
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"Value %s not usable\n", argv[opt]);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No value for option \"-L\"given\n");
               exit(EXIT_FAILURE);
            }
            s_stabilization_extra = (int)val;
            break;
         case 'o':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            str = argv[opt];
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"Value %s not usable as an offset\n", argv[opt]);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No value for the offset given\n");
               exit(EXIT_FAILURE);
            }
            s_offset = (int)val;
            break;
         case 'r':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            str = argv[opt];
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"Value %s not usable as the number of rounds for \"-r\"\n", argv[opt]);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No value for the number of rounds given\n");
               exit(EXIT_FAILURE);
            }
            s_number_of_test_loops = (int)val;
            break;

         case 'M':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            str = argv[opt];
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"Value %s not usable as the upper limit of T-C tests (\"-M\")\n", argv[opt]);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No value for the upper limit of T-C tests given\n");
               exit(EXIT_FAILURE);
            }
            upper_limit_print = (int)val;
            break;
         case 'm':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            str = argv[opt];
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"Value %s not usable as the increment for the T-C tests (\"-m\")\n", argv[opt]);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No value for the increment for the T-C tests given\n");
               exit(EXIT_FAILURE);
            }
            increment_print = (int)val;
            break;
         case 's':
            printpreset = 1;
            print = 1;
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            str = argv[opt];
            i = 0;
            /* Only the most basic checks */
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"input #%d wrong\n", i+1);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No input for #%d?\n", i+1);
               exit(EXIT_FAILURE);
            }
            i++;
            str = endptr + 1;
            KARATSUBA_MUL_CUTOFF = (int)val;
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"input #%d wrong\n", i+1);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No input for #%d?\n", i+1);
               exit(EXIT_FAILURE);
            }
            i++;
            str = endptr + 1;
            KARATSUBA_SQR_CUTOFF = (int)val;
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"input #%d wrong\n", i+1);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No input for #%d?\n", i+1);
               exit(EXIT_FAILURE);
            }
            i++;
            str = endptr + 1;
            TOOM_MUL_CUTOFF = (int)val;
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"input #%d wrong\n", i+1);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No input for #%d?\n", i+1);
               exit(EXIT_FAILURE);
            }
            i++;
            str = endptr + 1;
            TOOM_SQR_CUTOFF = (int)val;
            break;
         case 'h':
         default:
            s_usage(argv[0]);
            exit(EXIT_FAILURE);
         }
      }
   }

   /*
     mp_rand uses the cryptographically secure
     source of the OS by default. That is too expensive, too slow and
     most important for a benchmark: it is not repeatable.
   */
   s_mp_rand_jenkins_init(seed);
   mp_rand_source(s_mp_rand_jenkins);

   ksm  = KARATSUBA_MUL_CUTOFF;
   kss  = KARATSUBA_SQR_CUTOFF;
   tc3m = TOOM_MUL_CUTOFF;
   tc3s = TOOM_SQR_CUTOFF;

   if ((bncore == 0) && (printpreset == 0)) {
      /* Turn all limits from bncore.c to the max */
      KARATSUBA_MUL_CUTOFF = INT_MAX;
      KARATSUBA_SQR_CUTOFF = INT_MAX;
      TOOM_MUL_CUTOFF = INT_MAX;
      TOOM_SQR_CUTOFF = INT_MAX;
#ifdef BN_S_MP_KARATSUBA_MUL_C
      /*
         The influence of the Comba multiplication cannot be
         eradicated programmatically. It depends on the size
         of the macro MP_WPARRAY in tommath.h which needs to
         be changed manually (to 0 (zero)).
       */
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Karatsuba multiplication.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         KARATSUBA_MUL_CUTOFF = INT_MAX;
         t1 = s_time_mul(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Karatsuba multiplication  failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         KARATSUBA_MUL_CUTOFF = x;
         t2 = s_time_mul(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Karatsuba multiplication failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == s_stabilization_extra) {
               count = 0;
               break;
            } else if (count < s_stabilization_extra) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      KARATSUBA_MUL_CUTOFF = x - s_stabilization_extra * increment_print;
#endif
#ifdef BN_S_MP_KARATSUBA_SQR_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Karatsuba squaring.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         KARATSUBA_SQR_CUTOFF = INT_MAX;
         t1 = s_time_sqr(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Karatsuba squaring failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         KARATSUBA_SQR_CUTOFF = x;
         t2 = s_time_sqr(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Karatsuba squaring failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == s_stabilization_extra) {
               count = 0;
               break;
            } else if (count < s_stabilization_extra) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      KARATSUBA_SQR_CUTOFF = x - s_stabilization_extra * increment_print;
#endif
#ifdef BN_S_MP_TOOM_MUL_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Toom-Cook 3-way multiplying.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         TOOM_MUL_CUTOFF = INT_MAX;
         t1 = s_time_mul(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 3-way multiplying failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         TOOM_MUL_CUTOFF = x;
         t2 = s_time_mul(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 3-way multiplication failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == s_stabilization_extra) {
               count = 0;
               break;
            } else if (count < s_stabilization_extra) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      TOOM_MUL_CUTOFF = x - s_stabilization_extra * increment_print;
#endif
#ifdef BN_S_MP_TOOM_SQR_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Toom-Cook 3-way squaring.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         TOOM_SQR_CUTOFF = INT_MAX;
         t1 = s_time_sqr(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 3-way squaring failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         TOOM_SQR_CUTOFF = x;
         t2 = s_time_sqr(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 3-way squaring failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == s_stabilization_extra) {
               count = 0;
               break;
            } else if (count < s_stabilization_extra) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      TOOM_SQR_CUTOFF = x - s_stabilization_extra * increment_print;
#endif
   }
   if (terse == 1) {
      printf("%d %d %d %d\n",
             KARATSUBA_MUL_CUTOFF,
             KARATSUBA_SQR_CUTOFF,
             TOOM_MUL_CUTOFF,
             TOOM_SQR_CUTOFF);
   } else {
      printf("KARATSUBA_MUL_CUTOFF = %d\n", KARATSUBA_MUL_CUTOFF);
      printf("KARATSUBA_SQR_CUTOFF = %d\n", KARATSUBA_SQR_CUTOFF);
      printf("TOOM_MUL_CUTOFF = %d\n", TOOM_MUL_CUTOFF);
      printf("TOOM_SQR_CUTOFF = %d\n", TOOM_SQR_CUTOFF);
   }

   if (print == 1) {
      printf("Printing data for graphing to \"%s\" and \"%s\"\n",mullog, sqrlog);

      multiplying = fopen(mullog, "w+");
      if (multiplying == NULL) {
         fprintf(stderr, "Opening file \"%s\" failed\n", mullog);
         exit(EXIT_FAILURE);
      }

      squaring = fopen(sqrlog, "w+");
      if (squaring == NULL) {
         fprintf(stderr, "Opening file \"%s\" failed\n",sqrlog);
         exit(EXIT_FAILURE);
      }

      for (x = 8; x < upper_limit_print; x += increment_print) {
         KARATSUBA_MUL_CUTOFF = INT_MAX;
         KARATSUBA_SQR_CUTOFF = INT_MAX;
         TOOM_MUL_CUTOFF = INT_MAX;
         TOOM_SQR_CUTOFF = INT_MAX;
         t1 = s_time_mul(x);
         KARATSUBA_MUL_CUTOFF = kss;
         KARATSUBA_SQR_CUTOFF = ksm;
         TOOM_MUL_CUTOFF = tc3m;
         TOOM_SQR_CUTOFF = tc3s;
         t2 = s_time_mul(x);
         fprintf(multiplying, "%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         fflush(multiplying);
         if (verbose == 1) {
            printf("MUL %d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
            fflush(stdout);
         }
         KARATSUBA_MUL_CUTOFF = INT_MAX;
         KARATSUBA_SQR_CUTOFF = INT_MAX;
         TOOM_MUL_CUTOFF = INT_MAX;
         TOOM_SQR_CUTOFF = INT_MAX;
         t1 = s_time_sqr(x);
         KARATSUBA_MUL_CUTOFF = kss;
         KARATSUBA_SQR_CUTOFF = ksm;
         TOOM_MUL_CUTOFF = tc3m;
         TOOM_SQR_CUTOFF = tc3s;
         t2 = s_time_sqr(x);
         fprintf(squaring,"%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         fflush(squaring);
         if (verbose == 1) {
            printf("SQR %d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
            fflush(stdout);
         }
      }
      printf("Finished. Data for graphing in \"%s\" and \"%s\"\n",mullog, sqrlog);
      if (verbose == 1) {
         KARATSUBA_MUL_CUTOFF = kss;
         KARATSUBA_SQR_CUTOFF = ksm;
         TOOM_MUL_CUTOFF = tc3m;
         TOOM_SQR_CUTOFF = tc3s;
         if (terse == 1) {
            printf("%d %d %d %d\n",
                   KARATSUBA_MUL_CUTOFF,
                   KARATSUBA_SQR_CUTOFF,
                   TOOM_MUL_CUTOFF,
                   TOOM_SQR_CUTOFF);
         } else {
            printf("KARATSUBA_MUL_CUTOFF = %d\n", KARATSUBA_MUL_CUTOFF);
            printf("KARATSUBA_SQR_CUTOFF = %d\n", KARATSUBA_SQR_CUTOFF);
            printf("TOOM_MUL_CUTOFF = %d\n", TOOM_MUL_CUTOFF);
            printf("TOOM_SQR_CUTOFF = %d\n", TOOM_SQR_CUTOFF);
         }
      }
   }
   exit(EXIT_SUCCESS);
}
