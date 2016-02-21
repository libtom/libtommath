#include <tommath.h>
#ifdef BN_MP_SUBFACTORIAL_C

#ifndef MP_DIGIT_SIZE
#define MP_DIGIT_SIZE (1L<<DIGIT_BIT)
#endif

static int SUBFACTORIAL_CUTOFF = 5000;

static int subfactorial_iterative(mp_digit n, mp_int *c)
{
  mp_int r;
  int err;
  int one;
  mp_digit k;

  err = MP_OKAY;

  if ((err = mp_init_set(&r, (mp_digit) 1)) != MP_OKAY) {
    return err;
  }
  if (n == 0 || n == 2) {
    mp_exch(&r, c);
    goto _ERR;
  }
  if (n == 1) {
    mp_zero(&r);
    mp_exch(&r, c);
    goto _ERR;
  }
  one = 1;
  for (k = 3; k <= n; k++) {
    one = -one;
    if ((err = mp_mul_d(&r, k, &r)) != MP_OKAY) {
      goto _ERR;
    }
    if ((err = mp_add_d(&r, one, &r)) != MP_OKAY) {
      goto _ERR;
    }
  }
  mp_exch(&r, c);
_ERR:
  mp_clear(&r);
  return err;
}
// Shamelessly stolen from libtomfloat
static int binsplit(mp_int * a, mp_int * b, mp_int * P, mp_int * Q)
{
    int err;
    mp_int p1, q1, p2, q2, t1, one;
    if ((err = mp_init_multi(&p1, &q1, &p2, &q2, &t1, &one, NULL)) != MP_OKAY) {
        return err;
    }

    err = MP_OKAY;
    mp_set(&one, 1);
    if ((err = mp_sub(b, a, &t1)) != MP_OKAY) {
        goto _ERR;
    }
    if (mp_cmp(&t1, &one) == MP_EQ) {
        if ((err = mp_set_int(P, 1)) != MP_OKAY) {
            goto _ERR;
        }
        if ((err = mp_copy(b, Q)) != MP_OKAY) {
            goto _ERR;
        }
        goto _ERR;
    }
    if ((err = mp_add(a, b, &t1)) != MP_OKAY) {
        goto _ERR;
    }
    if ((err = mp_div_2d(&t1, 1, &t1, NULL)) != MP_OKAY) {
        goto _ERR;
    }

    if ((err = binsplit(a, &t1, &p1, &q1)) != MP_OKAY) {
        goto _ERR;
    }
    if ((err = binsplit(&t1, b, &p2, &q2)) != MP_OKAY) {
        goto _ERR;
    }
    //P = q2*p1 + p2
    if ((err = mp_mul(&q2, &p1, &t1)) != MP_OKAY) {
        goto _ERR;
    }
    if ((err = mp_add(&t1, &p2, P)) != MP_OKAY) {
        goto _ERR;
    }
    //Q =  q1*q2
    if ((err = mp_mul(&q1, &q2, Q)) != MP_OKAY) {
        goto _ERR;
    }

  _ERR:
    mp_clear_multi(&p1, &q1, &p2, &q2, &t1, &one, NULL);
    return err;
}

static int subfactorial_binsplit(unsigned long n, mp_int * c)
{
  mp_int p, q, zero, N;
  int err;
  err = MP_OKAY;

  mp_init_multi(&p, &q, &zero, &N, NULL);
  mp_set_int(&N, n);
  mp_zero(&zero);
  if ((err = binsplit(&zero, &N, &p, &q)) != MP_OKAY) {
    goto _ERR;
  }

  if ((err = mp_sqr(&q, &N)) != MP_OKAY) {
    goto _ERR;
  }
  if ((err = mp_add(&N, &q, &N)) != MP_OKAY) {
    goto _ERR;
  }
  if ((err = mp_add(&p, &q, &p)) != MP_OKAY) {
    goto _ERR;
  }
  if ((err = mp_div(&N, &p, &p, NULL)) != MP_OKAY) {
    goto _ERR;
  }
  mp_exch(&p, c);
_ERR:
  mp_clear_multi(&p, &q, &zero, &N, NULL);
  return err;
}

int mp_subfactorial(unsigned long n, mp_int * result)
{
  if (n <= (unsigned long)SUBFACTORIAL_CUTOFF && n < MP_DIGIT_SIZE)
    return subfactorial_iterative((mp_digit)n, result);
  else
    return subfactorial_binsplit(n, result);
}

#endif
