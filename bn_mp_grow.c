/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is library that provides for multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library is designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@iahu.ca, http://math.libtomcrypt.org
 */
#include <tommath.h>

/* grow as required */
int
mp_grow (mp_int * a, int size)
{
  int     i, n;

  /* if the alloc size is smaller alloc more ram */
  if (a->alloc < size) {
    size += (MP_PREC * 2) - (size & (MP_PREC - 1));	/* ensure there are always at least MP_PREC digits extra on top */

    a->dp = realloc (a->dp, sizeof (mp_digit) * size);
    if (a->dp == NULL) {
      return MP_MEM;
    }

    n = a->alloc;
    a->alloc = size;
    for (i = n; i < a->alloc; i++) {
      a->dp[i] = 0;
    }
  }
  return MP_OKAY;
}
