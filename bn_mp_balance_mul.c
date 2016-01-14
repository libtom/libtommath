#include <tommath.h>
#ifdef BN_MP_BALANCE_MUL_C

int mp_balance_mul(mp_int * a, mp_int * b, mp_int * c)
{

  int e, count, len_a, len_b, nblocks, i;
  mp_int a0, tmp;
  len_a = a->used;
  len_b = b->used;

  /* Make sure that a is the larger one but don't forget it */
  if (len_a < len_b) {
    mp_exch(a, b);
  }

  /* single-digit multiplication with b as the single-digit */

  nblocks = MAX(a->used, b->used) / MIN(a->used, b->used);

  if ((e = mp_init_size(&a0, b->used + 1)) != MP_OKAY) {
    return e;
  }
  
  if ((e = mp_init(&tmp)) != MP_OKAY) {
    mp_clear(&a0);
    return e;
  }
  mp_zero(c);
  for (i = 0; i < nblocks; i++) {
    /* Cut a slice off of a */
    a0.used = 0;
    for (count = 0; count < b->used; count++) {
      a0.dp[count] = a->dp[count + i * b->used];
      a0.used++;
    }
    mp_clamp(&a0);
    /* Multiply with b */
    if ((e = mp_mul(&a0, b, &tmp)) != MP_OKAY) {
      goto ERR;
    }
    /* Shift tmp to the correct position */
    if ((e = mp_lshd(&tmp, b->used * i)) != MP_OKAY) {
      goto ERR;
    }
    /* Add to output. No carry needed */
    if ((e = mp_add(c, &tmp, c)) != MP_OKAY) {
      goto ERR;
    }
  }
  /* The left-overs, because there are always left-overs */
  a0.used = 0;
  for (count = 0; count < a->used - (i * b->used); count++) {
    a0.dp[count] = a->dp[count + i * b->used];
    a0.used++;
  }
  mp_clamp(&a0);

  if ((e = mp_mul(&a0, b, &tmp)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_lshd(&tmp, b->used * i)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(c, &tmp, c)) != MP_OKAY) {
    goto ERR;
  }

  if (len_a < len_b) {
    mp_exch(a, b);
  }

ERR:
  mp_clear_multi(&a0, &tmp, NULL);

  return MP_OKAY;
}

#endif

