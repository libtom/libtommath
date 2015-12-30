#include <tommath.h>
#ifdef BN_MP_SET_STR_C

#include <string.h>

static int SET_STR_CUTOFF = 20;
static int mp_set_str_intern(mp_int * a, const char *string, int base)
{
  int len, len_low, len_high;
  char *s_low, *s_high;
  mp_int A, B, m;
  int e = MP_OKAY;

  len = strlen(string);
  len_low = len / 2;
  len_high = len - len_low;

  if (len_low < SET_STR_CUTOFF) {
    if ((e = mp_read_radix(a, string, base)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }
  if ((e = mp_init_set(&m, base)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_init_multi(&A, &B, NULL)) != MP_OKAY) {
    mp_clear(&m);
    return e;
  }
  // Yes, but do _you_ want to do all of the necessary pointer juggling?
  s_high = malloc(sizeof(char) * (len_high + 1));
  if (s_high == NULL) {
    e = MP_MEM;
    goto _ERR;
  }
  memcpy(s_high, string + len_low, len_high);
  s_high[len_high] = '\0';
  if ((e = mp_set_str_intern(&A, s_high, base)) != MP_OKAY) {
    free(s_high);
    goto _ERR;
  }
  free(s_high);

  s_low = malloc(sizeof(char) * (len_low + 1));
  if (s_low == NULL) {
    e = MP_MEM;
    goto _ERR;
  }
  memcpy(s_low, string, len_low);
  s_low[len_low] = '\0';
  if ((e = mp_set_str_intern(&B, s_low, base)) != MP_OKAY) {
    free(s_low);
    goto _ERR;
  }
  free(s_low);
  // TODO: caching?
  if ((e = mp_expt_d(&m, len_high, &m)) != MP_OKAY) {
    goto _ERR;
  }
  if ((e = mp_mul(&B, &m, &B)) != MP_OKAY) {
    goto _ERR;
  }
  if ((e = mp_add(&A, &B, a)) != MP_OKAY) {
    goto _ERR;
  }

_ERR:
  mp_clear_multi(&A, &B, &m, NULL);
  return e;
}

int mp_set_str(mp_int * a, const char *string, int base)
{
  int sign;
  // TODO: trim string?
  if (*string == '-') {
    string++;
    sign = MP_NEG;
  } else {
    sign = MP_ZPOS;
  }
  if (*string == '+') {
    string++;
  }
  mp_set_str_intern(a, string, base);
  a->sign = sign;
  return MP_OKAY;
}


#endif
