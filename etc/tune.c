/* Tune the Karatsuba parameters
 *
 * Tom St Denis, tomstdenis@iahu.ca
 */
#include <tommath.h>
#include <time.h>

clock_t
time_mult (void)
{
  clock_t   t1;
  int       x, y;
  mp_int    a, b, c;

  mp_init (&a);
  mp_init (&b);
  mp_init (&c);

  t1 = clock ();
  for (x = 8; x <= 128; x += 8) {
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
  clock_t   t1;
  int       x, y;
  mp_int    a, b;

  mp_init (&a);
  mp_init (&b);

  t1 = clock ();
  for (x = 8; x <= 128; x += 8) {
    mp_rand (&a, x);
    for (y = 0; y < 10000; y++) {
      mp_sqr (&a, &b);
    }
  }
  mp_clear (&a);
  mp_clear (&b);
  return clock () - t1;
}

int
main (void)
{
  int       best_mult, best_square;
  clock_t   best, ti;

  best_mult = best_square = 0;

  /* tune multiplication first */
  best = CLOCKS_PER_SEC * 1000;
  for (KARATSUBA_MUL_CUTOFF = 8; KARATSUBA_MUL_CUTOFF <= 128;
       KARATSUBA_MUL_CUTOFF++) {
    ti = time_mult ();
    printf ("%4d : %9lu\r", KARATSUBA_MUL_CUTOFF, ti);
    fflush (stdout);
    if (ti < best) {
      printf ("New best: %lu, %d         \n", ti, KARATSUBA_MUL_CUTOFF);
      best = ti;
      best_mult = KARATSUBA_MUL_CUTOFF;
    }
  }

  /* tune squaring */
  best = CLOCKS_PER_SEC * 1000;
  for (KARATSUBA_SQR_CUTOFF = 8; KARATSUBA_SQR_CUTOFF <= 128;
       KARATSUBA_SQR_CUTOFF++) {
    ti = time_sqr ();
    printf ("%4d : %9lu\r", KARATSUBA_SQR_CUTOFF, ti);
    fflush (stdout);
    if (ti < best) {
      printf ("New best: %lu, %d         \n", ti, KARATSUBA_SQR_CUTOFF);
      best = ti;
      best_square = KARATSUBA_SQR_CUTOFF;
    }
  }

  printf
    ("\n\n\nKaratsuba Multiplier Cutoff: %d\nKaratsuba Squaring Cutoff: %d\n",
     best_mult, best_square);

  return 0;
}
