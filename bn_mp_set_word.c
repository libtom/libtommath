#include <tommath.h>
#ifdef BN_MP_SET_WORD_C
#if (MP_PREC > 1)
int mp_set_word(mp_int * c, mp_word w)
{
  mp_zero(c);
  if (w == 0) {
    return MP_OKAY;
  }
  do {
    c->dp[c->used++] = (mp_digit) w & MP_MASK;
  } while ((w >>= DIGIT_BIT) > 0 && c->used < MP_PREC);
  if (w != 0) {
    return MP_VAL;
  }
  return MP_OKAY;
}
#else
#   warning "MP_PREC" should be at least 2 (two), better 3 (three)
#endif

#endif
