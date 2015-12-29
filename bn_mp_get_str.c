#include <tommath.h>
#ifdef BN_MP_GET_STR_C

#   include <string.h>
#   include <stdint.h>

#   if (defined _ITS_64_BIT_)
static const int tab64[64] = {
  63, 0, 58, 1, 59, 47, 53, 2,
  60, 39, 48, 27, 54, 33, 42, 3,
  61, 51, 37, 40, 49, 18, 28, 20,
  55, 30, 34, 11, 43, 14, 22, 4,
  62, 57, 46, 52, 38, 26, 32, 41,
  50, 36, 17, 19, 29, 10, 13, 21,
  56, 45, 25, 31, 35, 16, 9, 12,
  44, 24, 15, 8, 23, 7, 6, 5
};

static int ilog2(uint64_t value)
{
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  value |= value >> 32;
  return tab64[((uint64_t) ((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >>
	       58];
}
#   elif (defined _ITS_32_BIT_)
static const int tab32[32] = {
  0, 9, 1, 10, 13, 21, 2, 29,
  11, 14, 16, 18, 22, 25, 3, 30,
  8, 12, 20, 28, 15, 17, 24, 7,
  19, 27, 23, 6, 26, 5, 4, 31
};

static int ilog2(uint32_t value)
{
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  return tab32[(uint32_t) (value * 0x07C4ACDD) >> 27];
}
#   else
static int ilog2(int value)
{
  int r = 0;
  while ((value >>= 1) != 0) {
    r++;
  }
  return r;
}
#   endif


static int SCHOENHAGE_CONVERSION_CUT = 10;

static mp_int *schoenhagecache;
static int schoenhagecache_len;
static const int log_table[65] = {
  0, 0, 1, 1, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4,
  4, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6
};


static int mp_get_str_intern(mp_int * a, char *string, int digits, int base)
{
  char *str, *s0;
  int b, n;
  int ed;
  int err, i;
  size_t size;
  mp_int q, r;
  if (a->used <= SCHOENHAGE_CONVERSION_CUT) {
    size = (size_t) (mp_digits(a, 10) + 10);
    str = malloc(size * sizeof(char));
    if (NULL == str) {
      fprintf(stderr, "malloc failed to allocate %lu bytes\n",
	      size * sizeof(char));
      return MP_MEM;
    }
    s0 = str;
    while (--size) {
      *s0++ = '\0';
    }
    if ((err = mp_toradix(a, str, base)) != MP_OKAY) {
      return err;
    }
    if ((strlen(str) < (unsigned) digits) && (strlen(string) > 0)) {
      for (i = strlen(str); i < digits; i++) {
	string = strncat(string, "0", 1);
      }
    }
    string = strcat(string, str);
    free(str);
    return MP_OKAY;
  }

  b = mp_count_bits(a);
  n = ilog2(b / (2 * log_table[base])) - 1;

  if (n >= (int) (sizeof(int) * CHAR_BIT) - 1) {
    return MP_VAL;
  }

  if (schoenhagecache_len == 0) {
    schoenhagecache = malloc(32 * sizeof(mp_int));
    if (schoenhagecache == NULL) {
      return MP_MEM;
    }
    mp_init_set(&(schoenhagecache[0]), (mp_digit) (base));
    for (i = 1; i < n; i++) {
      if ((err = mp_init(&(schoenhagecache[i]))) != MP_OKAY) {
	return err;
      }
      if ((err =
	   mp_sqr(&(schoenhagecache[i - 1]),
		  &(schoenhagecache[i]))) != MP_OKAY) {
	return err;
      }
    }
    schoenhagecache_len = i;
  }
  if (n >= schoenhagecache_len) {
    for (i = schoenhagecache_len; i <= n; i++) {
      if ((err = mp_init(&(schoenhagecache[i]))) != MP_OKAY) {
	return err;
      }
      if ((err =
	   mp_sqr(&(schoenhagecache[i - 1]),
		  &(schoenhagecache[i]))) != MP_OKAY) {
	return err;
      }
    }
    schoenhagecache_len = i;
  }
  if ((err = mp_init_multi(&q, &r, NULL)) != MP_OKAY) {
    return err;
  }

  if ((err = mp_div(a, &(schoenhagecache[n]), &q, &r)) != MP_OKAY) {
    mp_clear_multi(&q, &r, NULL);
    return err;
  }
  // TODO: 1<<n can overflow, check (but it's good for over 3 billion
  //       decimal digits at 32 bit)
  ed = 1 << n;
  if ((err = mp_get_str_intern(&q, string, digits - ed, base)) != MP_OKAY) {
    mp_clear_multi(&q, &r, NULL);
    return err;
  }
  if ((err = mp_get_str_intern(&r, string, ed, base)) != MP_OKAY) {
    mp_clear_multi(&q, &r, NULL);
    return err;
  }
  mp_clear_multi(&q, &r, NULL);
  return MP_OKAY;
}

void free_schoenhage_cache()
{
  int i;
  for (i = 0; i < schoenhagecache_len; i++)
    mp_clear(&(schoenhagecache[i]));
  free(schoenhagecache);
}

//TODO: needs one char more than necessary for the number
int mp_get_str(mp_int * a, char *string, int digits, int base)
{
  int sign, e;
  // we need a defined starting point
  *string = '\0';
  sign = a->sign;
  if (sign == MP_NEG) {
    *string = '-';
    string++;
    *string = '\0';
  }
  a->sign = MP_ZPOS;
  if ((e = mp_get_str_intern(a, string, digits, base)) != MP_OKAY) {
    return e;
  }
  a->sign = sign;
  return MP_OKAY;
}


#endif

