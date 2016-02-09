#include <tommath.h>
#ifdef BN_MP_FACTORIAL_C

static long highbit(unsigned long n)
{
  long r = 0;
  while (n >>= 1) {
    r++;
  }
  return r;
}

unsigned long mp_prime_divisors(unsigned long n, unsigned long p)
{
  unsigned long q, m;
  q = n;
  m = 0LU;
  if (p > n)
    return 0LU;
  if (p > n / 2LU)
    return 1LU;
  while (q >= p) {
    q = q / p;
    m += q;
  }
  return m;
}

static int primorial__lowlevel(unsigned long *array, unsigned long n,
			       mp_int * result)
{
  unsigned long int i, first_half, second_half;
  mp_int tmp;
  int e;

  if (n == 0) {
    mp_set(result, 1);
    return MP_OKAY;
  }
  // Do the rest linearily. Faster for primorials at least,  but YMMV
  if (n <= 64) {
    mp_set(result, array[0]);
    for (i = 1; i < n; i++)
      mp_mul_d(result, array[i], result);
    return MP_OKAY;
  }

  first_half = n / 2;
  second_half = n - first_half;
  if ((e = primorial__lowlevel(array, second_half, result)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_init(&tmp)) != MP_OKAY) {
    return e;
  }
  if ((e =
       primorial__lowlevel(array + second_half, first_half, &tmp)) != MP_OKAY) {
    mp_clear(&tmp);
    return e;
  }
  if ((e = mp_mul(result, &tmp, result)) != MP_OKAY) {
    mp_clear(&tmp);
    return e;
  }
  mp_clear(&tmp);
  return MP_OKAY;
}

/*
static void print_q(unsigned long *q,unsigned long n)
{
   unsigned long i;
   printf("%lu - ",n);
   for (i=0; i<n; i++)
      printf("%lu *",q[i]);
   puts("");
}
*/

#   define MP_FACTORIAL_BORW_LOOP_CUTOFF 500000
#   define MP_FACTORIAL_BORW_PRIMORIAL_CUTOFF 200000
static int factorial_borwein(unsigned long n, mp_int * result)
{
  unsigned long *p_list, *arr;
  unsigned long *exp_list;
  unsigned long p, i, j, cut;
  long bit;
  int shift, e;
  mp_int temp;

  unsigned long r = 0;
  p_list = mp_fill_prime_list(3, n + 1, &r);
  exp_list = malloc(sizeof(unsigned long) * (r + 1));
  if (exp_list == NULL) {
    return MP_MEM;
  }

  if ((e = mp_set_int(result, 1)) != MP_OKAY) {
    return e;
  }

  shift = (int) mp_prime_divisors(n, 2LU);

  cut = n / 2;

  for (p = 0; p < r; p++) {
    if (p_list[p] > cut)
      break;
    exp_list[p] = mp_prime_divisors(n, p_list[p]);
  }
  if ((e = mp_init(&temp)) != MP_OKAY) {
    return e;
  }
  bit = (int) highbit(exp_list[0]);
  if (n < MP_FACTORIAL_BORW_LOOP_CUTOFF) {
    for (; bit >= 0; bit--) {
      if ((e = mp_sqr(result, result)) != MP_OKAY) {
	mp_clear(&temp);
	return e;
      }
      for (i = 0; i < p; i++) {
	if ((exp_list[i] & (1 << bit)) != 0) {
	  if ((e = mp_mul_d(result, (mp_digit) p_list[i], result)) != MP_OKAY) {
	    mp_clear(&temp);
	    return e;
	  }
	}
      }
    }
  } else {
    for (; bit >= 0; bit--) {
      arr = malloc(sizeof(unsigned long) * (r + 1));
      if (arr == NULL) {
	mp_clear(&temp);
	return MP_MEM;
      }
      if ((e = mp_sqr(result, result)) != MP_OKAY) {
	mp_clear(&temp);
	free(arr);
	return e;
      }
      if ((e = mp_set_int(&temp, 1)) != MP_OKAY) {
	mp_clear(&temp);
	free(arr);
	return e;
      }
      for (i = 0, j = 0; i < p; i++) {
	if ((exp_list[i] & (1 << bit)) != 0) {
	  /*
	   * if ((e = mp_mul_d(&temp, (mp_digit) p_list[i], &temp)) != MP_OKAY) {
	   * return e;
	   * }
	   */
	  arr[j++] = p_list[i];
	}
      }
      primorial__lowlevel(arr, j, &temp);
      if ((e = mp_mul(&temp, result, result)) != MP_OKAY) {
	mp_clear(&temp);
	free(arr);
	return e;
      }
      free(arr);
    }
  }
  if (n < MP_FACTORIAL_BORW_PRIMORIAL_CUTOFF) {
    for (; p < r; p++) {
      if ((e = mp_mul_d(result, (mp_digit) p_list[p], result)) != MP_OKAY) {
	mp_clear(&temp);
	return e;
      }
    }
  } else {
    if ((e = mp_primorial(cut, n, &temp)) != MP_OKAY) {
      mp_clear(&temp);
      return e;
    }
    if ((e = mp_mul(result, &temp, result)) != MP_OKAY) {
      mp_clear(&temp);
      return e;
    }
  }
  if ((e = mp_mul_2d(result, shift, result)) != MP_OKAY) {
    mp_clear(&temp);
    return e;
  }
  mp_clear(&temp);
  return MP_OKAY;
}

static int fbinsplit2b(unsigned long n, unsigned long m, mp_int * temp)
{
  mp_int t1;
  int e;
  unsigned long k;
  if (m <= (n + 1)) {
    if ((e = mp_set_int(temp, n)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }
  if (m == (n + 2)) {
    if ((e = mp_set_int(temp, n * m)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }
  k = (n + m) / 2;
  if ((k & 1) != 1)
    k = k - 1;
  if ((e = mp_init(&t1)) != MP_OKAY) {
    return e;
  }
  if ((e = fbinsplit2b(n, k, temp)) != MP_OKAY) {
    mp_clear(&t1);
    return e;
  }
  if ((e = mp_copy(temp, &t1)) != MP_OKAY) {
    mp_clear(&t1);
    return e;
  }
  if ((e = fbinsplit2b(k + 2, m, temp)) != MP_OKAY) {
    mp_clear(&t1);
    return e;
  }
  if ((e = mp_mul(&t1, temp, temp)) != MP_OKAY) {
    mp_clear(&t1);
    return e;
  }
  mp_clear(&t1);
  return MP_OKAY;
}

static int fbinsplit2a(unsigned long n, mp_int * p, mp_int * r)
{
  mp_int temp;
  int e;
  if (n <= 2)
    return MP_OKAY;
  if ((e = fbinsplit2a(n / 2, p, r)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_init(&temp)) != MP_OKAY) {
    mp_clear(&temp);
    return e;
  }
  if ((e =
       fbinsplit2b(n / 2 + 1 + ((n / 2) & 1), n - 1 + (n & 1),
		   &temp)) != MP_OKAY) {
    mp_clear(&temp);
    return e;
  }
  if ((e = mp_mul(p, &temp, p)) != MP_OKAY) {
    mp_clear(&temp);
    return e;
  }
  mp_clear(&temp);
  if ((e = mp_mul(r, p, r)) != MP_OKAY) {
    return e;
  }
  return MP_OKAY;
}

static int factorial_binsplit(unsigned long n, mp_int * result)
{
  mp_int p, r;
  int e;
  if ((e = mp_init_multi(&p, &r, NULL)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set_int(&p, 1)) != MP_OKAY) {
    mp_clear_multi(&p, &r, NULL);
    return e;
  }
  if ((e = mp_set_int(&r, 1)) != MP_OKAY) {
    mp_clear_multi(&p, &r, NULL);
    return e;
  }
  if ((e = fbinsplit2a(n, &p, &r)) != MP_OKAY) {
    mp_clear_multi(&p, &r, NULL);
    return e;
  }
  if ((e = mp_mul_2d(&r, (int) mp_prime_divisors(n, 2LU), result)) != MP_OKAY) {
    mp_clear_multi(&p, &r, NULL);
    return e;
  }
  mp_clear_multi(&p, &r, NULL);
  return MP_OKAY;
}

#   ifndef MP_DIGIT_SIZE
#      define MP_DIGIT_SIZE (1L<<DIGIT_BIT)
#   endif
static int factorial_naive(unsigned long n, mp_int * result)
{
  unsigned long m, l = 1, k = 0;
  int e;
  if ((e = mp_set_int(result, 1)) != MP_OKAY) {
    return e;
  }
  for (; n > 1; n--) {
    for (m = n; !(m & 0x1); m >>= 1)
      k++;
    if (l <= MP_DIGIT_SIZE / m) {
      l *= m;
      continue;
    }
    if ((e = mp_mul_d(result, l, result)) != MP_OKAY) {
      return e;
    }
    l = m;
  }
  if (l > 1) {
    if ((e = mp_mul_d(result, l, result)) != MP_OKAY) {
      return e;
    }
  }

  if ((e = mp_mul_2d(result, k, result)) != MP_OKAY) {
    return e;
  }
  return MP_OKAY;
}

int mp_factorial(unsigned long n, mp_int * result)
{
  switch (n) {
  case 0:
  case 1:
    return mp_set_int(result, 1);
    break;
  case 2:
    return mp_set_int(result, 2);
    break;
  case 3:
    return mp_set_int(result, 6);
    break;
  case 4:
    return mp_set_int(result, 24);
    break;
  case 5:
    return mp_set_int(result, 120);
    break;
#   if (defined MP_16BIT) || (defined MP_28BIT)\
 || (defined MP_31BIT) || (defined MP_64BIT)
  case 6:
    return mp_set_int(result, 720);
    break;
  case 7:
    return mp_set_int(result, 5040);
    break;
  case 8:
    return mp_set_int(result, 40320);
    break;
#   endif
#   if (defined MP_28BIT)\
 || (defined MP_31BIT)|| (defined MP_64BIT)
  case 9:
    return mp_set_int(result, 362880LU);
    break;
  case 10:
    return mp_set_int(result, 3628800LU);
    break;
  case 11:
    return mp_set_int(result, 39916800LU);
    break;
#   endif
#   if (defined MP_31BIT)|| (defined MP_64BIT)
  case 12:
    return mp_set_int(result, 479001600LLU);
    break;
#   endif
#   if (defined MP_64BIT)
  case 13:
    return mp_set_int(result, 6227020800LLU);
    break;
  case 14:
    return mp_set_int(result, 87178291200LLU);
    break;
  case 15:
    return mp_set_int(result, 1307674368000LLU);
    break;
  case 16:
    return mp_set_int(result, 20922789888000LLU);
    break;
  case 17:
    return mp_set_int(result, 355687428096000LLU);
    break;
  case 18:
    return mp_set_int(result, 6402373705728000LLU);
    break;
  case 19:
    return mp_set_int(result, 121645100408832000LLU);
    break;
#   endif
  default:
    break;
  }
  //return factorial_borwein(n, result);
  if (n < 1700) {
    return factorial_naive(n, result);
  } else if (n < 65536) {
    return factorial_binsplit(n, result);
  }

  return factorial_borwein(n, result);
}

#endif

