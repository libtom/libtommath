#include <tommath.h>
#ifdef BN_MP_FIBONACCI_C
/* max n = 47 */
static unsigned long fibonacci(unsigned long n)
{
  unsigned long i = 1, j = 0, k, l;
  for (k = 1; k <= n; k++) {
    l = i + j;
    i = j;
    j = l;
  }
  return j;
}

/* matrix expo. */
int mp_fibonacci(unsigned long n, mp_int * r)
{
  unsigned long i = n - 1;
  mp_int a, b, c, d, t, t1, t2, t3;
  int e;

  if ((e = mp_init_multi(&a, &b, &c, &d, &t, &t1, &t2, &t3, NULL)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&a, (mp_digit) 1)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&b, (mp_digit) 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&c, (mp_digit) 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&d, (mp_digit) 1)) != MP_OKAY) {
    return e;
  }

  while (i > 0) {
    if (i & 0x1) {
      //t = d*(a + b) + c*b;
      if ((e = mp_mul(&c, &b, &t1)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_add(&a, &b, &t2)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_mul(&d, &t2, &t3)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_add(&t3, &t1, &t)) != MP_OKAY) {
	return e;
      }
      //a = d*b + c*a;
      if ((e = mp_mul(&d, &b, &t1)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_mul(&c, &a, &t2)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_add(&t1, &t2, &a)) != MP_OKAY) {
	return e;
      }
      //b = t;
      if ((e = mp_copy(&t, &b)) != MP_OKAY) {
	return e;
      }
    }
    //t = d*(2*c + d);
    if ((e = mp_mul_2d(&c, 1, &t1)) != MP_OKAY) {
      return e;
    }
    if ((e = mp_add(&t1, &d, &t2)) != MP_OKAY) {
      return e;
    }
    if ((e = mp_mul(&d, &t2, &t)) != MP_OKAY) {
      return e;
    }
    //c = c*c + d*d;
    if ((e = mp_sqr(&c, &t1)) != MP_OKAY) {
      return e;
    }
    if ((e = mp_sqr(&d, &t2)) != MP_OKAY) {
      return e;
    }
    if ((e = mp_add(&t1, &t2, &c)) != MP_OKAY) {
      return e;
    }
    //d = t;
    if ((e = mp_copy(&t, &d)) != MP_OKAY) {
      return e;
    }
    i /= 2;
  }
  if ((e = mp_add(&a, &b, r)) != MP_OKAY) {
    return e;
  }
  mp_clear_multi(&a, &b, &c, &d, &t, &t1, &t2, &t3, NULL);
  return MP_OKAY;
}
#endif
