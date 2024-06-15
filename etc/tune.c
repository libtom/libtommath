/* Tune the Karatsuba parameters
 *
 * Tom St Denis, tstdenis82@gmail.com
 */
#include "tommath_private.h"
#include <time.h>
#include <inttypes.h>
#include <errno.h>

#define S_MP_RAND_JENKINS_C
#include "../demo/s_mp_rand_jenkins.c"

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

#define s_mp_mul_full(a, b, c) s_mp_mul(a, b, c, (a)->used + (b)->used + 1)
static uint64_t s_time_mul(int size)
{
   int x;
   mp_err  e;
   mp_int  a, b, c, d;
   uint64_t t1;

   if ((e = mp_init_multi(&a, &b, &c, &d, NULL)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR;
   }

   if ((e = mp_rand(&a, size * s_offset)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR;
   }
   if ((e = mp_rand(&b, size)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR;
   }

   s_timer_start();
   for (x = 0; x < s_number_of_test_loops; x++) {
      if ((e = mp_mul(&a,&b,&c)) != MP_OKAY) {
         t1 = UINT64_MAX;
         goto LBL_ERR;
      }
      if (s_check_result == 1) {
         if ((e = s_mp_mul_full(&a,&b,&d)) != MP_OKAY) {
            t1 = UINT64_MAX;
            goto LBL_ERR;
         }
         if (mp_cmp(&c, &d) != MP_EQ) {
            /* Time of 0 cannot happen (famous last words?) */
            t1 = 0u;
            goto LBL_ERR;
         }
      }
   }

   t1 = s_timer_stop();
LBL_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return t1;
}

static uint64_t s_time_sqr(int size)
{
   int x;
   mp_err  e;
   mp_int  a, b, c;
   uint64_t t1;

   if ((e = mp_init_multi(&a, &b, &c, NULL)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR;
   }

   if ((e = mp_rand(&a, size)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR;
   }

   s_timer_start();
   for (x = 0; x < s_number_of_test_loops; x++) {
      if ((e = mp_sqr(&a,&b)) != MP_OKAY) {
         t1 = UINT64_MAX;
         goto LBL_ERR;
      }
      if (s_check_result == 1) {
         if ((e = s_mp_sqr(&a,&c)) != MP_OKAY) {
            t1 = UINT64_MAX;
            goto LBL_ERR;
         }
         if (mp_cmp(&c, &b) != MP_EQ) {
            t1 = 0u;
            goto LBL_ERR;
         }
      }
   }

   t1 = s_timer_stop();
LBL_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return t1;
}

/* Set cutoff for radix conversion (base 10 only for now but should be good enough) */
#include <stdlib.h>
static mp_err random_number(char **string, size_t length)
{
   char alphabet[] = "0123456789", *str_cpy;

   *string = malloc(length + 1);
   if (*string == NULL) {
      return MP_MEM;
   }
   str_cpy = *string;
   /* No leading zeros */
   do {
      *str_cpy = alphabet[rand() % 10];
   } while (*str_cpy == '0');
   length--;
   str_cpy++;

   do {
      *str_cpy = alphabet[rand() % 10];
      str_cpy++;
   } while (--length > 0);

   *str_cpy = '\0';

   return MP_OKAY;
}

#include <string.h>
static uint64_t s_time_radix_conversion_read(int size)
{
   int x;
   size_t length;
   size_t written;
   mp_err  err;
   mp_int  a;
   char *str_a, *str_b;
   uint64_t t1;

   /* "size" is given as "number of limbs" and starts at 8 */
   length = (size_t)(size * MP_DIGIT_BIT);

   /* Over-estimate number of base 10 digits
      Magick number: 28/93 = CF(log_10(2))_(p_3, q_3)
    */
   written = (length * 28u);
   /* May happen e.g. if size > 2184  with MP_16BIT
      but cutoff should be about a couple of thousand bits
      at most (around or above Karatsuba cutoff).
    */
   if (length != written / 28u) {
      t1 = UINT64_MAX;
      goto LBL_ERR_1;
   }
   length = written / 93u + 2u;

   if ((err = random_number(&str_a, length)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR_1;
   }

   if ((err = mp_init(&a)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR_2;
   }
   s_timer_start();
   for (x = 0; x < s_number_of_test_loops; x++) {
      if ((err = mp_read_radix(&a, str_a, 10)) != MP_OKAY) {
         t1 = UINT64_MAX;
         goto LBL_ERR_3;
      }
   }
   t1 = s_timer_stop();

   if ((err = mp_radix_size(&a, 10, &length)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR_3;
   }

   str_b = malloc(length + 1);
   if (str_b == NULL) {
      t1 = UINT64_MAX;
      goto LBL_ERR_3;
   }
   if ((err = mp_to_radix(&a, str_b, length, &written, 10)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR;
   }

   if (strcmp(str_a, str_b) != 0) {
      t1 = 0u;
      goto LBL_ERR;
   }

LBL_ERR:
   free(str_b);
LBL_ERR_3:
   mp_clear(&a);
LBL_ERR_2:
   free(str_a);
LBL_ERR_1:
   return t1;
}

static uint64_t s_time_radix_conversion_write(int size)
{
   int x;
   size_t written, length;
   mp_err  err;
   mp_int  a, b;
   char *str_a;
   uint64_t t1;


   if ((err = mp_init_multi(&a, &b, NULL)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR_1;
   }
   if ((err = mp_rand(&a, size)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR_2;
   }

   if ((err = mp_radix_size(&a, 10, &length)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR_2;
   }

   str_a = malloc(length + 1);
   if (str_a == NULL) {
      t1 = UINT64_MAX;
      goto LBL_ERR_2;
   }

   s_timer_start();
   for (x = 0; x < s_number_of_test_loops; x++) {
      if ((err =  mp_to_radix(&a, str_a, length, &written, 10)) != MP_OKAY) {
         t1 = UINT64_MAX;
         goto LBL_ERR_2;
      }
   }
   t1 = s_timer_stop();

   if ((err = mp_read_radix(&b, str_a, 10)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LBL_ERR;
   }

   if (mp_cmp(&a, &b) != MP_EQ) {
      t1 = 0u;
      goto LBL_ERR;
   }


LBL_ERR:
   free(str_a);
LBL_ERR_2:
   mp_clear_multi(&a, &b, NULL);
LBL_ERR_1:
   return t1;
}


struct tune_args {
   int testmode;
   int verbose;
   int print;
   int bncore;
   int terse;
   int upper_limit_print;
   int increment_print;
} args;

static void s_run(const char *name, uint64_t (*op)(int size), int *cutoff)
{
   int x, count = 0;
   uint64_t t1, t2;
   if ((args.verbose == 1) || (args.testmode == 1)) {
      printf("# %s.\n", name);
   }
   for (x = 8; x < args.upper_limit_print; x += args.increment_print) {
      *cutoff = INT_MAX;
      t1 = op(x);
      if ((t1 == 0u) || (t1 == UINT64_MAX)) {
         fprintf(stderr,"%s failed at x = INT_MAX (%s)\n", name,
                 (t1 == 0u)?"wrong result":"internal error");
         exit(EXIT_FAILURE);
      }
      *cutoff = x;
      t2 = op(x);
      if ((t2 == 0u) || (t2 == UINT64_MAX)) {
         fprintf(stderr,"%s failed (%s)\n", name,
                 (t2 == 0u)?"wrong result":"internal error");
         exit(EXIT_FAILURE);
      }
      if (args.verbose == 1) {
         printf("%d: %9" PRIu64 " %9" PRIu64 ", %9" PRIi64 "\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
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
   *cutoff = x - s_stabilization_extra * args.increment_print;
}

static long s_strtol(const char *str, char **endptr, const char *err)
{
   const int base = 10;
   char *_endptr;
   long val;
   errno = 0;
   val = strtol(str, &_endptr, base);
   if ((val > INT_MAX || val < 0) || (errno != 0)) {
      fprintf(stderr, "Value %s not usable\n", str);
      exit(EXIT_FAILURE);
   }
   if (_endptr == str) {
      fprintf(stderr, "%s\n", err);
      exit(EXIT_FAILURE);
   }
   if (endptr != NULL) *endptr = _endptr;
   return val;
}

static int s_exit_code = EXIT_FAILURE;
static void s_usage(char *s)
{
   fprintf(stderr,"Usage: %s [TvcpGbtrSLFfMmosh]\n",s);
   fprintf(stderr,"          -T testmode, for use with testme.sh\n");
   fprintf(stderr,"          -v verbose, print all timings\n");
   fprintf(stderr,"          -c check results\n");
   fprintf(stderr,"          -p print benchmark of final cutoffs in files \"multiplying\",\n");
   fprintf(stderr,"             \"squaring\", \"readradix\", and  \"writeradix\" \n");
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
   fprintf(stderr,"             ksm, kss, tc3m, tc3s, rcr, rcw in that order\n");
   fprintf(stderr,"             ksm  = karatsuba multiplication\n");
   fprintf(stderr,"             kss  = karatsuba squaring\n");
   fprintf(stderr,"             tc3m = Toom-Cook 3-way multiplication\n");
   fprintf(stderr,"             tc3s = Toom-Cook 3-way squaring\n");
   fprintf(stderr,"             rcr = Fast radix conversion, reading\n");
   fprintf(stderr,"             rcw = Fast radix conversion, writing\n");
   fprintf(stderr,"             Implies '-p'\n");
   fprintf(stderr,"          -h this message\n");
   exit(s_exit_code);
}

struct cutoffs {
   int MUL_KARATSUBA, SQR_KARATSUBA;
   int MUL_TOOM, SQR_TOOM;
   int RADIX_READ, RADIX_WRITE;
};

const struct cutoffs max_cutoffs =
{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,INT_MAX, INT_MAX };

static void set_cutoffs(const struct cutoffs *c)
{
   MP_MUL_KARATSUBA_CUTOFF = c->MUL_KARATSUBA;
   MP_SQR_KARATSUBA_CUTOFF = c->SQR_KARATSUBA;
   MP_MUL_TOOM_CUTOFF = c->MUL_TOOM;
   MP_SQR_TOOM_CUTOFF = c->SQR_TOOM;
   MP_RADIX_READ_CUTOFF = c->RADIX_READ;
   MP_RADIX_WRITE_CUTOFF = c->RADIX_WRITE;
}

static void get_cutoffs(struct cutoffs *c)
{
   c->MUL_KARATSUBA  = MP_MUL_KARATSUBA_CUTOFF;
   c->SQR_KARATSUBA  = MP_SQR_KARATSUBA_CUTOFF;
   c->MUL_TOOM = MP_MUL_TOOM_CUTOFF;
   c->SQR_TOOM = MP_SQR_TOOM_CUTOFF;
   c->RADIX_READ = MP_RADIX_READ_CUTOFF;
   c->RADIX_WRITE = MP_RADIX_WRITE_CUTOFF;
}

int main(int argc, char **argv)
{
   uint64_t t1, t2;
   int x, i, j;
   size_t n;

   int printpreset = 0;
   /*int preset[8];*/
   char *endptr, *str;

   uint64_t seed = 0xdeadbeefULL;

   int opt;
   struct cutoffs orig, updated;

   FILE *squaring, *multiplying, *readradix, *writeradix;
   char mullog[256] = "multiplying";
   char sqrlog[256] = "squaring";
   char rcreadlog[256] = "readradix";
   char rcwritelog[256] = "writeradix";
   s_number_of_test_loops = 64;
   s_stabilization_extra = 3;

   s_mp_zero_buf(&args, sizeof(args));

   args.testmode = 0;
   args.verbose = 0;
   args.print = 0;
   args.bncore = 0;
   args.terse = 0;

   args.upper_limit_print = 3000;
   args.increment_print = 1;

   /* Very simple option parser, please treat it nicely. */
   if (argc != 1) {
      for (opt = 1; (opt < argc) && (argv[opt][0] == '-'); opt++) {
         switch (argv[opt][1]) {
         case 'T':
            args.testmode = 1;
            s_check_result = 1;
            args.upper_limit_print = 1000;
            args.increment_print = 11;
            s_number_of_test_loops = 1;
            s_stabilization_extra = 1;
            s_offset = 1;
            break;
         case 'v':
            args.verbose = 1;
            break;
         case 'c':
            s_check_result = 1;
            break;
         case 'p':
            args.print = 1;
            break;
         case 'G':
            args.print = 1;
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

            for (i = 0; i < 255; i++) {
               if (rcreadlog[i] == '\0') {
                  break;
               }
            }
            for (j = 0; i < 255; j++, i++) {
               rcreadlog[i] = argv[opt][j];
               if (argv[opt][j] == '\0') {
                  break;
               }
            }

            for (i = 0; i < 255; i++) {
               if (rcwritelog[i] == '\0') {
                  break;
               }
            }
            for (j = 0; i < 255; j++, i++) {
               rcwritelog[i] = argv[opt][j];
               if (argv[opt][j] == '\0') {
                  break;
               }
            }

            break;
         case 'b':
            args.bncore = 1;
            break;
         case 't':
            args.terse = 1;
            break;
         case 'S':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            str = argv[opt];
            errno = 0;
            seed = (uint64_t)s_strtol(argv[opt], NULL, "No seed given?\n");
            break;
         case 'L':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            s_stabilization_extra = (int)s_strtol(argv[opt], NULL, "No value for option \"-L\"given");
            break;
         case 'o':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            s_offset = (int)s_strtol(argv[opt], NULL, "No value for the offset given");
            break;
         case 'r':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            s_number_of_test_loops = (int)s_strtol(argv[opt], NULL, "No value for the number of rounds given");
            break;

         case 'M':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            args.upper_limit_print = (int)s_strtol(argv[opt], NULL, "No value for the upper limit of T-C tests given");
            break;
         case 'm':
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            args.increment_print = (int)s_strtol(argv[opt], NULL, "No value for the increment for the T-C tests given");
            break;
         case 's':
            printpreset = 1;
            args.print = 1;
            opt++;
            if (opt >= argc) {
               s_usage(argv[0]);
            }
            str = argv[opt];
            MP_MUL_KARATSUBA_CUTOFF = (int)s_strtol(str, &endptr, "[1/6] No value for MP_MUL_KARATSUBA_CUTOFF given");
            str = endptr + 1;
            MP_SQR_KARATSUBA_CUTOFF = (int)s_strtol(str, &endptr, "[2/6] No value for MP_SQR_KARATSUBA_CUTOFF given");
            str = endptr + 1;
            MP_MUL_TOOM_CUTOFF = (int)s_strtol(str, &endptr, "[3/6] No value for MP_MUL_TOOM_CUTOFF given");
            str = endptr + 1;
            MP_SQR_TOOM_CUTOFF = (int)s_strtol(str, &endptr, "[4/6] No value for MP_SQR_TOOM_CUTOFF given");
            str = endptr + 1;
            MP_RADIX_READ_CUTOFF = (int)s_strtol(str, &endptr, "[5/6] No value for MP_RADIX_READ_CUTOFF given");
            str = endptr + 1;
            MP_RADIX_WRITE_CUTOFF = (int)s_strtol(str, &endptr, "[6/6] No value for MP_RADIX_WRITE_CUTOFF given");
            break;
         case 'h':
            s_exit_code = EXIT_SUCCESS;
         /* FALLTHROUGH */
         default:
            s_usage(argv[0]);
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

   get_cutoffs(&orig);

   updated = max_cutoffs;
   if ((args.bncore == 0) && (printpreset == 0)) {
      struct {
         const char *name;
         int *cutoff, *update;
         uint64_t (*fn)(int size);
      } test[] = {
#define T_MUL_SQR(n, o, f)  { #n, &MP_##o##_CUTOFF, &(updated.o), MP_HAS(S_MP_##o) ? f : NULL }
         /*
            The influence of the Comba multiplication cannot be
            eradicated programmatically. It depends on the size
            of the macro MP_WPARRAY in tommath.h which needs to
            be changed manually (to 0 (zero)).
          */
         T_MUL_SQR("Karatsuba multiplication", MUL_KARATSUBA, s_time_mul),
         T_MUL_SQR("Karatsuba squaring", SQR_KARATSUBA, s_time_sqr),
         T_MUL_SQR("Toom-Cook 3-way multiplying", MUL_TOOM, s_time_mul),
         T_MUL_SQR("Toom-Cook 3-way squaring", SQR_TOOM, s_time_sqr),
         /* TODO: adapt macro above (or the names of the cutoffs and/or functions) */
         {
            "\"Faster radix conversion (reading)\"", &MP_RADIX_READ_CUTOFF,
            &(updated.RADIX_READ),MP_HAS(S_MP_FASTER_READ_RADIX) ? s_time_radix_conversion_read : NULL
         },
         {
            "\"Faster radix conversion (writing)\"", &MP_RADIX_WRITE_CUTOFF,
            &(updated.RADIX_WRITE),MP_HAS(S_MP_FASTER_TO_RADIX) ? s_time_radix_conversion_write : NULL
         }

#undef T_MUL_SQR
      };
      /* Turn all limits from bncore.c to the max */
      set_cutoffs(&max_cutoffs);

      for (n = 0; n < (sizeof(test)/sizeof(test[0]) - 2); ++n) {
         if (test[n].fn != NULL) {
            s_run(test[n].name, test[n].fn, test[n].cutoff);
            *test[n].update = *test[n].cutoff;
         };
      }

      /* Cutoffs for radix conversions are in bits to make handling of 62 different radices easier  */
      for (; n < sizeof(test)/sizeof(test[0]); ++n) {
         if (test[n].fn != NULL) {
            s_run(test[n].name, test[n].fn, test[n].cutoff);
            *test[n].update = (*test[n].cutoff) * MP_DIGIT_BIT;
            *test[n].cutoff = INT_MAX;
         }
      }

   }
   if (printpreset == 1) {
      updated.MUL_KARATSUBA = MP_MUL_KARATSUBA_CUTOFF;
      updated.SQR_KARATSUBA = MP_SQR_KARATSUBA_CUTOFF;
      updated.MUL_TOOM = MP_MUL_TOOM_CUTOFF;
      updated.SQR_TOOM = MP_SQR_TOOM_CUTOFF;
      updated.RADIX_READ = MP_RADIX_READ_CUTOFF;
      updated.RADIX_WRITE = MP_RADIX_WRITE_CUTOFF;
   }
   if (args.terse == 1) {
      printf("%d %d %d %d %d %d\n",
             updated.MUL_KARATSUBA,
             updated.SQR_KARATSUBA,
             updated.MUL_TOOM,
             updated.SQR_TOOM,
             updated.RADIX_READ,
             updated.RADIX_WRITE);
   } else {
      printf("MUL_KARATSUBA_CUTOFF = %d\n", updated.MUL_KARATSUBA);
      printf("SQR_KARATSUBA_CUTOFF = %d\n", updated.SQR_KARATSUBA);
      printf("MUL_TOOM_CUTOFF = %d\n", updated.MUL_TOOM);
      printf("SQR_TOOM_CUTOFF = %d\n", updated.SQR_TOOM);
      printf("RADIX_READ_CUTOFF = %d\n", updated.RADIX_READ);
      printf("RADIX_WRITE_CUTOFF = %d\n", updated.RADIX_WRITE);
   }
   /* TODO: add graphs for radix conversion, too? */
   if (args.print == 1) {
      printf("Printing up to %d datapoints for graphing to \"%s\", \"%s\", \"%s\", and \"%s\"\n",args.upper_limit_print,
             mullog, sqrlog, rcreadlog, rcwritelog);

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

      readradix = fopen(rcreadlog, "w+");
      if (readradix == NULL) {
         fprintf(stderr, "Opening file \"%s\" failed\n",rcreadlog);
         exit(EXIT_FAILURE);
      }

      writeradix = fopen(rcwritelog, "w+");
      if (readradix == NULL) {
         fprintf(stderr, "Opening file \"%s\" failed\n",rcwritelog);
         exit(EXIT_FAILURE);
      }


      for (x = 1; x < args.upper_limit_print; x += args.increment_print) {
         printf("\r%d", x);
         fflush(stdout);
         set_cutoffs(&max_cutoffs);
         t1 = s_time_mul(x);
         set_cutoffs(&orig);
         t2 = s_time_mul(x);
         fprintf(multiplying, "%d: %9" PRIu64 " %9" PRIu64 ", %9" PRIi64 "\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         fflush(multiplying);
         if (args.verbose == 1) {
            printf("MUL %d: %9" PRIu64 " %9" PRIu64 ", %9" PRIi64 "\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
            fflush(stdout);
         }
         set_cutoffs(&max_cutoffs);
         t1 = s_time_sqr(x);
         set_cutoffs(&orig);
         t2 = s_time_sqr(x);
         fprintf(squaring,"%d: %9" PRIu64 " %9" PRIu64 ", %9" PRIi64 "\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         fflush(squaring);
         if (args.verbose == 1) {
            printf("SQR %d: %9" PRIu64 " %9" PRIu64 ", %9" PRIi64 "\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
            fflush(stdout);
         }
         /* The cutoffs are so low, we would see nothing interesting in the graphs with the default args.upper_limit_print */
         if ((x * MP_DIGIT_BIT) < (3 * updated.RADIX_READ)) {
            set_cutoffs(&max_cutoffs);
            t1 = s_time_radix_conversion_read(x);
            set_cutoffs(&orig);
            t2 = s_time_radix_conversion_read(x);
            fprintf(readradix,"%d: %9" PRIu64 " %9" PRIu64 ", %9" PRIi64 "\n", x * MP_DIGIT_BIT, t1, t2, (int64_t)t2 - (int64_t)t1);
            fflush(readradix);
            if (args.verbose == 1) {
               printf("RCR %d: %9" PRIu64 " %9" PRIu64 ", %9" PRIi64 "\n", x * MP_DIGIT_BIT, t1, t2, (int64_t)t2 - (int64_t)t1);
               fflush(stdout);
            }
         }

         if ((x * MP_DIGIT_BIT) < (5 * updated.RADIX_WRITE)) {
            set_cutoffs(&max_cutoffs);
            t1 = s_time_radix_conversion_write(x);
            set_cutoffs(&orig);
            t2 = s_time_radix_conversion_write(x);
            fprintf(writeradix,"%d: %9" PRIu64 " %9" PRIu64 ", %9" PRIi64 "\n", x * MP_DIGIT_BIT, t1, t2,
                    (int64_t)t2 - (int64_t)t1);
            fflush(writeradix);
            if (args.verbose == 1) {
               printf("RCW %d: %9" PRIu64 " %9" PRIu64 ", %9" PRIi64 "\n", x * MP_DIGIT_BIT, t1, t2, (int64_t)t2 - (int64_t)t1);
               fflush(stdout);
            }
         }

      }
      printf("Finished. Data for graphing in \"%s\", \"%s\", \"%s\", and \"%s\"\n",mullog, sqrlog, rcreadlog, rcwritelog);
      if (args.verbose == 1) {
         set_cutoffs(&orig);
         if (args.terse == 1) {
            printf("%d %d %d %d %d %d\n",
                   MP_MUL_KARATSUBA_CUTOFF,
                   MP_SQR_KARATSUBA_CUTOFF,
                   MP_MUL_TOOM_CUTOFF,
                   MP_SQR_TOOM_CUTOFF,
                   MP_RADIX_READ_CUTOFF,
                   MP_RADIX_WRITE_CUTOFF);
         } else {
            printf("MUL_KARATSUBA_CUTOFF = %d\n", MP_MUL_KARATSUBA_CUTOFF);
            printf("SQR_KARATSUBA_CUTOFF = %d\n", MP_SQR_KARATSUBA_CUTOFF);
            printf("MUL_TOOM_CUTOFF = %d\n", MP_MUL_TOOM_CUTOFF);
            printf("SQR_TOOM_CUTOFF = %d\n", MP_SQR_TOOM_CUTOFF);
            printf("MP_RADIX_READ_CUTOFF = %d\n", MP_RADIX_READ_CUTOFF);
            printf("MP_RADIX_WRITE_CUTOFF = %d\n", MP_RADIX_WRITE_CUTOFF);
         }
      }
      fclose(readradix);
      fclose(writeradix);
   }
   exit(EXIT_SUCCESS);
}
