#include <tommath.h>
#ifdef BN_MP_BALANCE_MUL_C

#if 0
#define MP_BMC1a (KARATSUBA_MUL_CUTOFF)
#define MP_BMC1b (MP_BMC1a)
#define MP_BMC2 0.001f
#define MP_BMC3 0.999f
#endif
int mp_balance_mul(mp_int * a, mp_int * b, mp_int * c)
{
#if 0
  int e, count, len_a, len_b;
  mp_int a_0, a_1;
  len_a = a->used;
  len_b = b->used;
  /* some shortcuts */
  /* Nothing to do, go on */
  if (len_a == len_b) {
    if ((e = mp_mul(a, b, c)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }
  /* 
    Check sizes. The smaller one needs to be larger than the Karatsuba cut-off,
    the bigger one needs some minimum, too, as some empiricals tests suggest.
   */
  if (MIN(len_a, len_b) < MP_BMC1a || MAX(len_a, len_b) < MP_BMC1b) {
    if ((e = mp_mul(a, b, c)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }

  /* check if the sizes of both differ enough in relation*/
  /* The first test is probably not necessary, may be even counter-productive */
  if ( ( ((float) MIN(len_a, len_b) / (float) MAX(len_a, len_b)) < MP_BMC2
      || ((float) MIN(len_a, len_b) / (float) MAX(len_a, len_b)) > MP_BMC3 )
      ) {
    if ((e = mp_mul(a, b, c)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }

  /* Make sure that a is the larger one but don't forget it */
  if (len_a < len_b) {
    mp_exch(a, b);
  }
  /* cut larger one in two parts a1, a0 with the smaller part a0 of the same
   * length as the smaller input number b_0. Work on copy to make things simpler
   */
  if ((e = mp_init_size(&a_0, b->used + 1)) != MP_OKAY) {
    return e;
  }
  a_0.used = b->used;
  if ((e = mp_init_size(&a_1, a->used - b->used + 1)) != MP_OKAY) {
    return e;
  }
  a_1.used = a->used - b->used;
  /* fill smaller part a_0 */
  for (count = 0; count < b->used; count++) {
    a_0.dp[count] = a->dp[count];
  }
  /* fill bigger part a_1 with the counter already at the right place */
  for (; count < a->used; count++) {
    a_1.dp[count - b->used] = a->dp[count];
  }
  /* a_1 = a_1 * b_0 */
  /* a_1 and b_0 are of different size and might allow another round */
  if ((a_1.used) > b->used && b->used > MP_BMC1a) {
    if ((e = mp_balance_mul(&a_1, b, &a_1)) != MP_OKAY) {
      return e;
    }
  } else {
    if ((e = mp_mul(&a_1, b, &a_1)) != MP_OKAY) {
      return e;
    }
  }
  /* a_1 = a_1 * 2^(length(b_0)) */
  if ((e = mp_lshd(&a_1, b->used)) != MP_OKAY) {
    return e;
  }
  /* a_0 = a_0 * b_0 */
  if ((e = mp_mul(&a_0, b, &a_0)) != MP_OKAY) {
    return e;
  }
  /* c = a_1 + a_0 */
  if ((e = mp_add(&a_1, &a_0, c)) != MP_OKAY) {
    return e;
  }
  /* Don't mess with the input more than necessary */
  if (len_a < len_b) {
    mp_exch(a, b);
  }
  mp_clear_multi(&a_0, &a_1, NULL);
#endif
  return MP_OKAY;
}

#endif
