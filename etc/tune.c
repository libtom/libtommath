/* Tune the Karatsuba parameters
 *
 * Tom St Denis, tomstdenis@iahu.ca
 */
#include <tommath.h>
#include <time.h>

clock_t
time_mult (void)
{
  clock_t t1;
  int     x, y;
  mp_int  a, b, c;

  mp_init (&a);
  mp_init (&b);
  mp_init (&c);

  t1 = clock ();
  for (x = 4; x <= 128; x += 4) {
    mp_rand (&a, x);
    mp_rand (&b, x);
    for (y = 0; y < 10000; y++) {
      mp_mul (&a, &b, &c);
    }
  }
  mp_clear (&a);
  mp_clear (&b);
  mp_clear (&c);
  return clock () - t1;
}

clock_t
time_sqr (void)
{
  clock_t t1;
  int     x, y;
  mp_int  a, b;

  mp_init (&a);
  mp_init (&b);

  t1 = clock ();
  for (x = 4; x <= 128; x += 4) {
    mp_rand (&a, x);
    for (y = 0; y < 10000; y++) {
      mp_sqr (&a, &b);
    }
  }
  mp_clear (&a);
  mp_clear (&b);
  return clock () - t1;
}

clock_t
time_expt (void)
{
  clock_t t1;
  int     x, y;
  mp_int  a, b, c, d;

  mp_init (&a);
  mp_init (&b);
  mp_init (&c);
  mp_init (&d);

  t1 = clock ();
  for (x = 4; x <= 128; x += 4) {
    mp_rand (&a, x);
    mp_rand (&b, x);
    mp_rand (&c, x);
    if (mp_iseven (&c) != 0) {
      mp_add_d (&c, 1, &c);
    }
    for (y = 0; y < 10; y++) {
      mp_exptmod (&a, &b, &c, &d);
    }
  }
  mp_clear (&d);
  mp_clear (&c);
  mp_clear (&b);
  mp_clear (&a);

  return clock () - t1;
}

int
main (void)
{
  int     best_mult, best_square, best_exptmod;
  clock_t best, ti;
  FILE   *log;

  best_mult = best_square = best_exptmod = 0;

  /* tune multiplication first */
  log = fopen ("mult.log", "w");
  best = CLOCKS_PER_SEC * 1000;
  for (KARATSUBA_MUL_CUTOFF = 8; KARATSUBA_MUL_CUTOFF <= 128; KARATSUBA_MUL_CUTOFF++) {
    ti = time_mult ();
    printf ("%4d : %9lu\r", KARATSUBA_MUL_CUTOFF, ti);
    fprintf (log, "%d, %lu\n", KARATSUBA_MUL_CUTOFF, ti);
    fflush (stdout);
    if (ti < best) {
      printf ("New best: %lu, %d         \n", ti, KARATSUBA_MUL_CUTOFF);
      best = ti;
      best_mult = KARATSUBA_MUL_CUTOFF;
    }
  }
  fclose (log);

  /* tune squaring */
  log = fopen ("sqr.log", "w");
  best = CLOCKS_PER_SEC * 1000;
  for (KARATSUBA_SQR_CUTOFF = 8; KARATSUBA_SQR_CUTOFF <= 128; KARATSUBA_SQR_CUTOFF++) {
    ti = time_sqr ();
    printf ("%4d : %9lu\r", KARATSUBA_SQR_CUTOFF, ti);
    fprintf (log, "%d, %lu\n", KARATSUBA_SQR_CUTOFF, ti);
    fflush (stdout);
    if (ti < best) {
      printf ("New best: %lu, %d         \n", ti, KARATSUBA_SQR_CUTOFF);
      best = ti;
      best_square = KARATSUBA_SQR_CUTOFF;
    }
  }
  fclose (log);

  /* tune exptmod */
  KARATSUBA_MUL_CUTOFF = best_mult;
  KARATSUBA_SQR_CUTOFF = best_square;

  log = fopen ("expt.log", "w");
  best = CLOCKS_PER_SEC * 1000;
  for (MONTGOMERY_EXPT_CUTOFF = 8; MONTGOMERY_EXPT_CUTOFF <= 192; MONTGOMERY_EXPT_CUTOFF++) {
    ti = time_expt ();
    printf ("%4d : %9lu\r", MONTGOMERY_EXPT_CUTOFF, ti);
    fflush (stdout);
    fprintf (log, "%d : %lu\r", MONTGOMERY_EXPT_CUTOFF, ti);
    if (ti < best) {
      printf ("New best: %lu, %d\n", ti, MONTGOMERY_EXPT_CUTOFF);
      best = ti;
      best_exptmod = MONTGOMERY_EXPT_CUTOFF;
    }
  }
  fclose (log);

  printf
    ("\n\n\nKaratsuba Multiplier Cutoff: %d\nKaratsuba Squaring Cutoff: %d\nMontgomery exptmod Cutoff: %d\n",
     best_mult, best_square, best_exptmod);

  return 0;
}
