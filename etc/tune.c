/* Tune the Karatsuba parameters
 *
 * Tom St Denis, tstdenis82@gmail.com
 */
#include "../tommath.h"
#include "../tommath_private.h"
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>

/*
   Some results for the author's machine, an AMD AMD A8-6600K (64-bit) with
   enough (6.5 GB for the CPU) but not very fast memory (DDR3 1600 MHz)
   Run under a cpuset (cset shield -c 0 -k on); memory isolation was not
   possible with that machine (UMA only) at least not easily.

   1000 runs each:

   60 bit MP_DIGIT:

          min median    mean  max        sd
     km    23    125 124.721  185 18.072514
     ks    32    128 127.649  128  4.222752
     tc3m  73    223 218.585  323 22.567500
     tc3s  63    258 256.205  533 27.911145
     tc4m 141    393 401.435  617 80.140691
     tc4s  92    386 366.561 1303 62.337182   <---
     tc5m 173    470 469.780  756 68.426982
     tc5s 129    382 364.158  800 48.332982   <---

           km  ks tc3m tc3s tc4m tc4s tc5m tc5s
     0%    23  32   73   63  141   92  173  129
     25%  122 128  213  258  365  358  426  338
     50%  125 128  223  258  393  386  470  382
     75%  131 128  223  258  421  386  525  382
     100% 185 128  323  533  617 1303  756  800

   LTM uses 128 bit integers for the MP_64BIT mp_word type which are rarely
   found in hardware. It might pay to use MP_32BIT instead for number-crunching.


   28 bit MP_DIGIT on a 64-bit machine:

          min median    mean max          sd
     km   134    257 256.178 257   7.0537984
     ks   128    128 128.021 134   0.3140982
     tc3m 203    513 499.855 553  37.2803406
     tc3s 253    258 260.265 298   5.0567129
     tc4m 309    764 704.682 827  87.5217398
     tc4s 253    386 384.957 421  14.7519752   <---
     tc5m 536    921 886.823 998 122.2362031
     tc5s 349    503 489.316 987  49.3219302   <---

           km  ks tc3m tc3s tc4m tc4s tc5m tc5s
     0%   134 128  203  253  309  253  536  349
     25%  257 128  508  258  666  386  789  492
     50%  257 128  513  258  764  386  921  503
     75%  257 128  513  258  764  386  998  503
     100% 257 134  553  298  827  421  998  987


   Please take in mind that both multiplicands are of the same size. The balancing
   mechanism in mp_balance works well but has some overhead itself. You can test
   the behaviour of it with the option "-o" followed by a (small) positive number 'x'
   to generate ratios of the form 1:x.

   But which ones to take? The median is most probably the best choice.

   The FFT tests are a bit special. There is no single lower cut-off point but
   several and the unavoidable rounding errors give a second upper cut-off point.
   The upper cut-off point depends on the size of the slices (between 5 and 14 bits)
   and the size of mp_word in relation to the size of the slices. The latter caused
   a large problem with MP_8BIT where the 16 bit of the mp_word type were not
   sufficient at all and needed the introduction of a 32-bit type. This, together
   with the use of double, might make it unusable for 8 bit but lack of actual
   hardware hindered the author from testing it in praxi.

   Several tries to automate those measurements failed (results too inexact), the
   best way for now is to print the curves with a very high upper limit and
   "eyeball" the best place for the lower cut-off.

   The upper FFT-limit where the rounding errors start to occur is very far away
   for the implementations with small slices. The smallest one is currently with
   MP_16BIT where the upper cut-off limit was found to be above 10,000,000 (On a
   64-bit machine, so YMMV as the saying goes). The 12 bit slice for MP-64BIT with
   60 bit limb-size has an upper limit on this machine at around 457,000 limbs, a
   magnitude of about 8,254,242 decimal digits.
   The biggest slice are for MP_28BIT with 14-bit slices (that type is also available
   with 7 bit slices) has an upper limit at about 171,000 (28 bit) limbs, a magnitude
   of about 1,441,331 decimal digits.

   The FFT functions have a build-in simple probabilistic self-check so using them
   up to their absolute limits is risky. It is highly advised to check the upper
   limit in advance and do a round or two of a Toom-Cook like splicing early enough.
   There is no automatism for that implemented, the exact points depend on too many
   variables.

   The tests for the FFT cut-offs are time consuming. You can switch them of by
   setting the upper limit to 0 (zero) with "-F 0" to do the T-C checks only.
*/

/* Bob Jenkins' http://burtleburtle.net/bob/rand/smallprng.html */
/* Chosen for speed and a good "mix" */
typedef struct ranctx {
   uint64_t a;
   uint64_t b;
   uint64_t c;
   uint64_t d;
} ranctx;

static ranctx burtle_x;

#   define rot(x,k) (((x)<<(k))|((x)>>(64-(k))))
static uint64_t ranval()
{
   uint64_t e = burtle_x.a - rot(burtle_x.b, 7);
   burtle_x.a = burtle_x.b ^ rot(burtle_x.c, 13);
   burtle_x.b = burtle_x.c + rot(burtle_x.d, 37);
   burtle_x.c = burtle_x.d + e;
   burtle_x.d = e + burtle_x.a;
   return burtle_x.d;
}

static void raninit(uint64_t seed)
{
   uint64_t i;
   burtle_x.a = 0xf1ea5eed;
   burtle_x.b = burtle_x.c = burtle_x.d = seed;
   for (i = 0; i < 20; ++i) {
      (void) ranval();
   }
}

/*
   The original used LTM's mp_rand which uses the cryptographically secure
   source of the OS for its purpose. That is too expensive, too slow and
   most important for a benchmark: it is not repeatable.
*/
static int s_mp_random(mp_int *a, int limbs)
{
   int e = MP_OKAY;
   if ((e = mp_grow(a, limbs + 1)) != MP_OKAY) {
      goto LTM_ERR;
   }
   a->used = limbs--;
   do {
      a->dp[limbs] = (mp_digit)(ranval() & MP_MASK);
   } while (limbs--);
   mp_clamp(a);
LTM_ERR:
   return e;
}



static uint64_t TIMFUNC(void)
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

/* *INDENT-OFF* */
/* generic ISO C timer */
static uint64_t LBL_T;
static void t_start(void) { LBL_T = TIMFUNC(); }
static uint64_t t_read(void) { return TIMFUNC() - LBL_T; }
/* *INDENT-ON* */

/* TODO: clean this mess up! Not just the following 4 lines, all of the below! */
static int check;
static int TIMES;
static int LTM_STABILIZE;
static int offset = 1;

#define s_mp_mul(a, b, c) s_mp_mul_digs(a, b, c, (a)->used + (b)->used + 1)
static uint64_t time_mul(int size)
{
   int x, e;
   mp_int  a, b, c, d;
   uint64_t t1;

   if ((e = mp_init_multi(&a, &b, &c, &d, NULL)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   if ((e = s_mp_random(&a, size * offset)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }
   if ((e = s_mp_random(&b, size)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   t_start();
   for (x = 0; x < TIMES; x++) {
      if ((e = mp_mul(&a,&b,&c)) != MP_OKAY) {
         t1 = UINT64_MAX;
         goto LTM_ERR;
      }
      if (check == 1) {
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

   t1 = t_read();
LTM_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return t1;
}

static uint64_t time_fft_mul(int size, int which)
{
   int x, e;
   mp_int  a, b, c, d;
   uint64_t t1;

   if ((e = mp_init_multi(&a, &b, &c, &d, NULL)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   if ((e = s_mp_random(&a, size * offset)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }
   if ((e = s_mp_random(&b, size)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }
   t_start();
   for (x = 0; x < TIMES; x++) {
      if (which == 1) {
         if ((e = s_mp_fft_mul(&a,&b,&c)) != MP_OKAY) {
            t1 = UINT64_MAX;
            goto LTM_ERR;
         }
         if (check == 1) {
            if ((e = mp_mul(&a,&b,&d)) != MP_OKAY) {
               t1 = UINT64_MAX;
               goto LTM_ERR;
            }
            if (mp_cmp(&c, &d) != MP_EQ) {
               t1 = 0uLL;
               goto LTM_ERR;
            }
         }
      } else {
         if ((e = mp_mul(&a,&b,&c)) != MP_OKAY) {
            t1 = UINT64_MAX;
            goto LTM_ERR;
         }
      }
   }

   t1 = t_read();
LTM_ERR:
   mp_clear_multi(&a, &b, &c, &d, NULL);
   return t1;
}


static uint64_t time_sqr(int size)
{
   int x, e;
   mp_int  a, b, c;
   uint64_t t1;

   if ((e = mp_init_multi(&a, &b, &c, NULL)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   if ((e = s_mp_random(&a, size)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   t_start();
   for (x = 0; x < TIMES; x++) {
      if ((e = mp_sqr(&a,&b)) != MP_OKAY) {
         t1 = UINT64_MAX;
         goto LTM_ERR;
      }
      if (check == 1) {
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

   t1 = t_read();
LTM_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return t1;
}

static uint64_t time_fft_sqr(int size, int which)
{
   int x, e;
   mp_int  a, b, c;
   uint64_t t1;

   if ((e = mp_init_multi(&a, &b, &c, NULL)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   if ((e = s_mp_random(&a, size)) != MP_OKAY) {
      t1 = UINT64_MAX;
      goto LTM_ERR;
   }

   t_start();
   for (x = 0; x < TIMES; x++) {
      if (which == 1) {
         if ((e = s_mp_fft_sqr(&a,&b)) != MP_OKAY) {
            t1 = UINT64_MAX;
            goto LTM_ERR;
         }
         if (check == 1) {
            if ((e = mp_sqr(&a,&c)) != MP_OKAY) {
               t1 = UINT64_MAX;
               goto LTM_ERR;
            }
            if (mp_cmp(&c, &b) != MP_EQ) {
               t1 = 0uLL;
               goto LTM_ERR;
            }
         }
      } else {
         if ((e = mp_sqr(&a, &c)) != MP_OKAY) {
            t1 = UINT64_MAX;
            goto LTM_ERR;
         }
      }
   }

   t1 = t_read();
LTM_ERR:
   mp_clear_multi(&a, &b, &c, NULL);
   return t1;
}

static void usage(char *s)
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
   fprintf(stderr,"          -t prints comma separated results\n");
   fprintf(stderr,"          -r [64] number of rounds\n");
   fprintf(stderr,"          -S [0xdeadbeef] seed for PRNG\n");
   fprintf(stderr,"          -L [3] number of negative values accumulated until the result is accepted\n");
   fprintf(stderr,"          -F [100000] upper limit of FFT tests/prints\n");
   fprintf(stderr,"          -f [1000] increment of FFT tests/prints\n");
   fprintf(stderr,"          -M [3000] upper limit of T-C tests/prints\n");
   fprintf(stderr,"          -m [7] increment of T-C tests/prints\n");
   fprintf(stderr,"          -o [1] multiplier for the second multiplicand\n");
   fprintf(stderr,"             (Not for computing the cut-offs!)\n");
   fprintf(stderr,"          -s 'preset' use values in 'preset' for printing.\n");
   fprintf(stderr,"             'preset' is a comma separated string with cut-offs for\n");
   fprintf(stderr,"             ksm, kss, tc3m, tc3s, tc4m, tc4s, tc5m, tc5s in that order\n");
   fprintf(stderr,"             ksm  = karatsuba multiplication\n");
   fprintf(stderr,"             kss  = karatsuba squaring\n");
   fprintf(stderr,"             tc3m = Toom-Cook 3-way multiplication\n");
   fprintf(stderr,"             tc3s = Toom-Cook 3-way squaring\n");
   fprintf(stderr,"             tc4m = Toom-Cook 4-way multiplication\n");
   fprintf(stderr,"             tc4s = Toom-Cook 4-way squaring\n");
   fprintf(stderr,"             tc5m = Toom-Cook 5-way multiplication\n");
   fprintf(stderr,"             tc5s = Toom-Cook 5-way squaring\n");
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

   int upper_limit_fft = 100000;
   int increment_fft = 1000;

   int upper_limit_print = 3000;
   int increment_print = 7;

   int printpreset = 0;
   /*int preset[8];*/
   int base = 10;
   char *endptr, *str;
   long val;

   uint64_t seed = 0xdeadbeef;

   int opt;
   int ksm, kss, tc3m, tc3s, tc4m, tc4s, tc5m, tc5s;

   FILE *squaring, *multiplying;
   char mullog[256] = "multiplying";
   char sqrlog[256] = "squaring";
   TIMES = 64;
   LTM_STABILIZE = 3;

   /* Very simple option parser, please treat it nicely. */
   if (argc != 1) {
      for (opt = 1; (opt < argc) && (argv[opt][0] == '-'); opt++) {
         switch (argv[opt][1]) {
         case 'T':
            testmode = 1;
            check = 1;
            upper_limit_fft = 1000;
            increment_fft = 10;
            upper_limit_print = 1000;
            increment_print = 11;
            TIMES = 1;
            LTM_STABILIZE = 1;
            offset = 1;
            break;
         case 'v':
            verbose = 1;
            break;
         case 'c':
            check = 1;
            break;
         case 'p':
            print = 1;
            break;
         case 'G':
            print = 1;
            opt++;
            if (opt >= argc) {
               usage(argv[0]);
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
               usage(argv[0]);
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
               usage(argv[0]);
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
            LTM_STABILIZE = (int)val;
            break;
         case 'o':
            opt++;
            if (opt >= argc) {
               usage(argv[0]);
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
            offset = (int)val;
            break;
         case 'r':
            opt++;
            if (opt >= argc) {
               usage(argv[0]);
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
            TIMES = (int)val;
            break;
         case 'F':
            opt++;
            if (opt >= argc) {
               usage(argv[0]);
            }
            str = argv[opt];
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"Value %s not usable as the upper limit of FFT tests (\"-F\")\n", argv[opt]);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No value for the upper limit of FFT tests given\n");
               exit(EXIT_FAILURE);
            }
            upper_limit_fft = (int)val;
            break;
         case 'f':
            opt++;
            if (opt >= argc) {
               usage(argv[0]);
            }
            str = argv[opt];
            errno = 0;
            val = strtol(str, &endptr, base);
            if ((val > INT_MAX || val < 0) || (errno != 0)) {
               fprintf(stderr,"Value %s not usable as the increment for the FFT tests (\"-f\")\n", argv[opt]);
               exit(EXIT_FAILURE);
            }
            if (endptr == str) {
               fprintf(stderr, "No value for the increment for the FFT tests given\n");
               exit(EXIT_FAILURE);
            }
            increment_fft = (int)val;
            break;

         case 'M':
            opt++;
            if (opt >= argc) {
               usage(argv[0]);
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
               usage(argv[0]);
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
               usage(argv[0]);
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
            TOOM_COOK_4_MUL_CO = (int)val;
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
            TOOM_COOK_4_SQR_CO = (int)val;
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
            TOOM_COOK_5_MUL_CO = (int)val;
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
            TOOM_COOK_5_SQR_CO = (int)val;
            break;
         case 'h':
         default:
            usage(argv[0]);
            exit(EXIT_FAILURE);
         }
      }
   }

   raninit(seed);

   ksm  = KARATSUBA_MUL_CUTOFF;
   kss  = KARATSUBA_SQR_CUTOFF;
   tc3m = TOOM_MUL_CUTOFF;
   tc3s = TOOM_SQR_CUTOFF;
   tc4m = TOOM_COOK_4_MUL_CO;
   tc4s = TOOM_COOK_4_SQR_CO;
   tc5m = TOOM_COOK_5_MUL_CO;
   tc5s = TOOM_COOK_5_SQR_CO;

   if ((bncore == 0) && (printpreset == 0)) {

      /* Turn all limits from bncore.c to the max */
      KARATSUBA_MUL_CUTOFF = INT_MAX;
      KARATSUBA_SQR_CUTOFF = INT_MAX;
      TOOM_MUL_CUTOFF = INT_MAX;
      TOOM_SQR_CUTOFF = INT_MAX;
      TOOM_COOK_4_SQR_CO = INT_MAX;
      TOOM_COOK_4_MUL_CO = INT_MAX;
      TOOM_COOK_5_SQR_CO = INT_MAX;
      TOOM_COOK_5_MUL_CO = INT_MAX;
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
         t1 = time_mul(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Karatsuba multiplication  failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         KARATSUBA_MUL_CUTOFF = x;
         t2 = time_mul(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Karatsuba multiplication failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == LTM_STABILIZE) {
               count = 0;
               break;
            } else if (count < LTM_STABILIZE) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      KARATSUBA_MUL_CUTOFF = x - LTM_STABILIZE * increment_print;
#endif
#ifdef BN_S_MP_KARATSUBA_SQR_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Karatsuba squaring.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         KARATSUBA_SQR_CUTOFF = INT_MAX;
         t1 = time_sqr(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Karatsuba squaring failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         KARATSUBA_SQR_CUTOFF = x;
         t2 = time_sqr(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Karatsuba squaring failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == LTM_STABILIZE) {
               count = 0;
               break;
            } else if (count < LTM_STABILIZE) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      KARATSUBA_SQR_CUTOFF = x - LTM_STABILIZE * increment_print;
#endif
#ifdef BN_S_MP_TOOM_MUL_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Toom-Cook 3-way multiplying.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         TOOM_MUL_CUTOFF = INT_MAX;
         t1 = time_mul(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 3-way multiplying failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         TOOM_MUL_CUTOFF = x;
         t2 = time_mul(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 3-way multiplication failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == LTM_STABILIZE) {
               count = 0;
               break;
            } else if (count < LTM_STABILIZE) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      TOOM_MUL_CUTOFF = x - LTM_STABILIZE * increment_print;
#endif
#ifdef BN_S_MP_TOOM_SQR_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Toom-Cook 3-way squaring.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         TOOM_SQR_CUTOFF = INT_MAX;
         t1 = time_sqr(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 3-way squaring failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         TOOM_SQR_CUTOFF = x;
         t2 = time_sqr(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 3-way squaring failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == LTM_STABILIZE) {
               count = 0;
               break;
            } else if (count < LTM_STABILIZE) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      TOOM_SQR_CUTOFF = x - LTM_STABILIZE * increment_print;
#endif
#ifdef BN_S_MP_TOOM_COOK_4_MUL_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Toom-Cook 4-way multiplying.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         TOOM_COOK_4_MUL_CO = INT_MAX;
         t1 = time_mul(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 4-way multiplying failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         TOOM_COOK_4_MUL_CO = x;
         t2 = time_mul(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 4-way multiplication failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == LTM_STABILIZE) {
               count = 0;
               break;
            } else if (count < LTM_STABILIZE) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      TOOM_COOK_4_MUL_CO = x - LTM_STABILIZE * increment_print;
#endif
#ifdef BN_S_MP_TOOM_COOK_4_SQR_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Toom-Cook 4-way squaring.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         TOOM_COOK_4_SQR_CO = INT_MAX;
         t1 = time_sqr(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 4-way squaring failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         TOOM_COOK_4_SQR_CO = x;
         t2 = time_sqr(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 4-way squaring failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == LTM_STABILIZE) {
               count = 0;
               break;
            } else if (count < LTM_STABILIZE) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      TOOM_COOK_4_SQR_CO = x - LTM_STABILIZE * increment_print;
#endif
#ifdef BN_S_MP_TOOM_COOK_5_MUL_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Toom-Cook 5-way multiplying.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         TOOM_COOK_5_MUL_CO = INT_MAX;
         t1 = time_mul(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 5-way multiplying failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         TOOM_COOK_5_MUL_CO = x;
         t2 = time_mul(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 5-way multiplication failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == LTM_STABILIZE) {
               count = 0;
               break;
            } else if (count < LTM_STABILIZE) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      TOOM_COOK_5_MUL_CO = x - LTM_STABILIZE * increment_print;
#endif
#ifdef BN_S_MP_TOOM_COOK_5_SQR_C
      if ((verbose == 1) || (testmode == 1)) {
         puts("# Toom-Cook 5-way squaring.");
      }
      for (x = 8; x < upper_limit_print; x += increment_print) {
         TOOM_COOK_5_SQR_CO = INT_MAX;
         t1 = time_sqr(x);
         if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 5-way squaring failed at x = INT_MAX (%s)\n",
                    (t1 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         TOOM_COOK_5_SQR_CO = x;
         t2 = time_sqr(x);
         if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
            fprintf(stderr,"Toom-Cook 5-way squaring failed (%s)\n",
                    (t2 == 0uLL)?"wrong result":"internal error");
            exit(EXIT_FAILURE);
         }
         if (verbose == 1) {
            printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         }
         if (t2 < t1) {
            if (count == LTM_STABILIZE) {
               count = 0;
               break;
            } else if (count < LTM_STABILIZE) {
               count++;
            }
         } else if (count > 0) {
            count--;
         }
      }
      TOOM_COOK_5_SQR_CO = x - LTM_STABILIZE * increment_print;
#endif
      ksm  = KARATSUBA_MUL_CUTOFF;
      kss  = KARATSUBA_SQR_CUTOFF;
      tc3m = TOOM_MUL_CUTOFF;
      tc3s = TOOM_SQR_CUTOFF;
      tc4m = TOOM_COOK_4_MUL_CO;
      tc4s = TOOM_COOK_4_SQR_CO;
      tc5m = TOOM_COOK_5_MUL_CO;
      tc5s = TOOM_COOK_5_SQR_CO;
   }

#ifdef BN_S_MP_FFT_MUL_C
   FFT_MUL_LOWER_CO = INT_MAX;
   FFT_MUL_UPPER_CO = INT_MAX;
   /*
      FFT is not recursive in the same sense as the Toom-Cook algorithms are
      and get their own tests.
   */
   if ((verbose == 1) || (testmode == 1)) {
      puts("# FFT multiplying.");
   }
   for (x = 8; x < upper_limit_fft; x += increment_fft) {
      t1 = time_fft_mul(x, 0);
      if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
         fprintf(stderr,"FFT multiplying failed: (%s)\n",
                 (t1 == 0uLL)?"wrong result":"internal error");
         if ((testmode == 1) || (check == 1)) {
            exit(EXIT_FAILURE);
         } else {
            break;
         }
      }
      t2 = time_fft_mul(x, 1);
      if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
         fprintf(stderr,"FFT multiplying failed (%s)\n",
                 (t2 == 0uLL)?"wrong result":"internal error");
         if ((testmode == 1) || (check == 1)) {
            exit(EXIT_FAILURE);
         } else {
            break;
         }
      }
      if (verbose == 1) {
         printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
      }
   }
#endif
#ifdef BN_S_MP_FFT_SQR_C
   FFT_SQR_LOWER_CO = INT_MAX;
   FFT_SQR_UPPER_CO = INT_MAX;
   if ((verbose == 1) || (testmode == 1)) {
      puts("# FFT squaring.");
   }
   for (x = 8; x < upper_limit_fft; x += increment_fft) {
      t1 = time_fft_sqr(x, 0);
      if ((t1 == 0uLL) || (t1 == UINT64_MAX)) {
         fprintf(stderr,"FFT squaring failed: (%s)\n",
                 (t1 == 0uLL)?"wrong result":"internal error");
         if ((testmode == 1) || (check == 1)) {
            exit(EXIT_FAILURE);
         } else {
            break;
         }
      }
      t2 = time_fft_sqr(x, 1);
      if ((t2 == 0uLL) || (t2 == UINT64_MAX)) {
         fprintf(stderr,"FFT squaring failed (%s)\n",
                 (t2 == 0uLL)?"wrong result":"internal error");
         if ((testmode == 1) || (check == 1)) {
            exit(EXIT_FAILURE);
         } else {
            break;
         }
      }
      if (verbose == 1) {
         printf("%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
      }
   }
#endif
   if (terse == 1) {
      printf("%d %d %d %d %d %d %d %d\n",
             KARATSUBA_MUL_CUTOFF,
             KARATSUBA_SQR_CUTOFF,
             TOOM_MUL_CUTOFF,
             TOOM_SQR_CUTOFF,
             TOOM_COOK_4_MUL_CO,
             TOOM_COOK_4_SQR_CO,
             TOOM_COOK_5_MUL_CO,
             TOOM_COOK_5_SQR_CO);
   } else {
      printf("KARATSUBA_MUL_CUTOFF = %d\n", KARATSUBA_MUL_CUTOFF);
      printf("KARATSUBA_SQR_CUTOFF = %d\n", KARATSUBA_SQR_CUTOFF);
      printf("TOOM_MUL_CUTOFF = %d\n", TOOM_MUL_CUTOFF);
      printf("TOOM_SQR_CUTOFF = %d\n", TOOM_SQR_CUTOFF);
      printf("TOOM_COOK_4_MUL_CO = %d\n", TOOM_COOK_4_MUL_CO);
      printf("TOOM_COOK_4_SQR_CO = %d\n", TOOM_COOK_4_SQR_CO);
      printf("TOOM_COOK_5_MUL_CO = %d\n", TOOM_COOK_5_MUL_CO);
      printf("TOOM_COOK_5_SQR_CO = %d\n", TOOM_COOK_5_SQR_CO);
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
         TOOM_COOK_4_SQR_CO = INT_MAX;
         TOOM_COOK_4_MUL_CO = INT_MAX;
         TOOM_COOK_5_SQR_CO = INT_MAX;
         TOOM_COOK_5_MUL_CO = INT_MAX;
         t1 = time_mul(x);
         KARATSUBA_MUL_CUTOFF = kss;
         KARATSUBA_SQR_CUTOFF = ksm;
         TOOM_MUL_CUTOFF = tc3m;
         TOOM_SQR_CUTOFF = tc3s;
         TOOM_COOK_4_MUL_CO = tc4m;
         TOOM_COOK_4_SQR_CO = tc4s;
         TOOM_COOK_5_MUL_CO = tc5m;
         TOOM_COOK_5_SQR_CO = tc5s;
         t2 = time_mul(x);
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
         TOOM_COOK_4_SQR_CO = INT_MAX;
         TOOM_COOK_4_MUL_CO = INT_MAX;
         TOOM_COOK_5_SQR_CO = INT_MAX;
         TOOM_COOK_5_MUL_CO = INT_MAX;
         t1 = time_sqr(x);
         KARATSUBA_MUL_CUTOFF = kss;
         KARATSUBA_SQR_CUTOFF = ksm;
         TOOM_MUL_CUTOFF = tc3m;
         TOOM_SQR_CUTOFF = tc3s;
         TOOM_COOK_4_MUL_CO = tc4m;
         TOOM_COOK_4_SQR_CO = tc4s;
         TOOM_COOK_5_MUL_CO = tc5m;
         TOOM_COOK_5_SQR_CO = tc5s;
         t2 = time_sqr(x);
         fprintf(squaring,"%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         fflush(squaring);
         if (verbose == 1) {
            printf("SQR %d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
            fflush(stdout);
         }
      }
#ifdef BN_S_MP_FFT_MUL_C
      fprintf(multiplying, "\nFFT MUL\n");
      for (x = 8; x < upper_limit_fft; x += increment_fft) {
         t1 = time_fft_mul(x, 0);
         t2 = time_fft_mul(x, 1);
         fprintf(multiplying, "%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         fflush(multiplying);
         if (verbose == 1) {
            printf("FFT MUL %d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
            fflush(stdout);
         }
      }
#endif
#ifdef BN_S_MP_FFT_SQR_C
      fprintf(squaring, "\nFFT SQR\n");
      for (x = 8; x < upper_limit_fft; x += increment_fft) {
         t1 = time_fft_sqr(x, 0);
         t2 = time_fft_sqr(x, 1);
         fprintf(squaring, "%d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
         fflush(squaring);
         if (verbose == 1) {
            printf("FFT SQR %d: %9"PRIu64" %9"PRIu64", %9"PRIi64"\n", x, t1, t2, (int64_t)t2 - (int64_t)t1);
            fflush(stdout);
         }
      }
#endif
      printf("Finished. Data for graphing in \"%s\" and \"%s\"\n",mullog, sqrlog);
      if (verbose == 1) {
         KARATSUBA_MUL_CUTOFF = kss;
         KARATSUBA_SQR_CUTOFF = ksm;
         TOOM_MUL_CUTOFF = tc3m;
         TOOM_SQR_CUTOFF = tc3s;
         TOOM_COOK_4_MUL_CO = tc4m;
         TOOM_COOK_4_SQR_CO = tc4s;
         TOOM_COOK_5_MUL_CO = tc5m;
         TOOM_COOK_5_SQR_CO = tc5s;
         if (terse == 1) {
            printf("%d %d %d %d %d %d %d %d\n",
                   KARATSUBA_MUL_CUTOFF,
                   KARATSUBA_SQR_CUTOFF,
                   TOOM_MUL_CUTOFF,
                   TOOM_SQR_CUTOFF,
                   TOOM_COOK_4_MUL_CO,
                   TOOM_COOK_4_SQR_CO,
                   TOOM_COOK_5_MUL_CO,
                   TOOM_COOK_5_SQR_CO);
         } else {
            printf("KARATSUBA_MUL_CUTOFF = %d\n", KARATSUBA_MUL_CUTOFF);
            printf("KARATSUBA_SQR_CUTOFF = %d\n", KARATSUBA_SQR_CUTOFF);
            printf("TOOM_MUL_CUTOFF = %d\n", TOOM_MUL_CUTOFF);
            printf("TOOM_SQR_CUTOFF = %d\n", TOOM_SQR_CUTOFF);
            printf("TOOM_COOK_4_MUL_CO = %d\n", TOOM_COOK_4_MUL_CO);
            printf("TOOM_COOK_4_SQR_CO = %d\n", TOOM_COOK_4_SQR_CO);
            printf("TOOM_COOK_5_MUL_CO = %d\n", TOOM_COOK_5_MUL_CO);
            printf("TOOM_COOK_5_SQR_CO = %d\n", TOOM_COOK_5_SQR_CO);
         }

      }
   }
   exit(EXIT_SUCCESS);
}
