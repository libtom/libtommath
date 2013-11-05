#include <tommath.h>
#ifdef BN_MP_PADOVAN_C
int mp_padovan(unsigned long n, mp_int * r)
{
  int i, j, k;
  mp_int a, b, c, d, e, f, g, h, p;
  mp_int t1, t2, t3, t4, t5, t6, t7, t8, t9;
  mp_int r1, r2, r3, r4, r5, r6, r7, r8, r9;
  mp_int T1;
  int E;
  mp_digit list[14] = { 1, 0, 0, 1, 0, 1, 1, 1, 2, 2, 3, 4, 5, 7 };
  if (n < 14) {
    mp_set(r, list[n]);
    return MP_OKAY;
  }

  n -= 3;

  if ((E = mp_init_multi(&a, &b, &c, &d, &e, &f, &g, &h, &p,
			 &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9,
			 &r1, &r2, &r3, &r4, &r5, &r6, &r7, &r8, &r9,
			 &T1, NULL)) != MP_OKAY) {
    return E;
  }

  mp_set(&a, 0);
  mp_set(&b, 1);
  mp_set(&c, 0);

  mp_set(&d, 0);
  mp_set(&e, 0);
  mp_set(&f, 1);

  mp_set(&g, 1);
  mp_set(&h, 1);
  mp_set(&p, 0);

  mp_set(&r1, 1);
  mp_set(&r2, 0);
  mp_set(&r3, 0);

  mp_set(&r4, 0);
  mp_set(&r5, 1);
  mp_set(&r6, 0);

  mp_set(&r7, 0);
  mp_set(&r8, 0);
  mp_set(&r9, 1);

  mp_int per[3][3] = { {a, b, c}
  , {d, e, f}
  , {g, h, p}
  };
  mp_int ret[3][3] = { {r1, r2, r3}
  , {r4, r5, r6}
  , {r7, r8, r9}
  };
  mp_int tmp[3][3] = { {t1, t2, t3}
  , {t4, t5, t6}
  , {t7, t8, t9}
  };

  while (n) {
    if (n & 1) {
      for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++) {
	  mp_set(&tmp[i][j], 0);
	}
      for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++)
	  for (k = 0; k < 3; k++) {
	    if ((E = mp_mul(&ret[i][k], &per[k][j], &T1)) != MP_OKAY) {
	      return E;
	    }
	    if ((E = mp_add(&T1, &tmp[i][j], &tmp[i][j])) != MP_OKAY) {
	      return E;
	    }
	  }
      for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++) {
	  if ((E = mp_copy(&tmp[i][j], &ret[i][j])) != MP_OKAY) {
	    return E;
	  }
	}
    }
    for (i = 0; i < 3; i++)
      for (j = 0; j < 3; j++) {
	mp_set(&tmp[i][j], 0);
      }
    for (i = 0; i < 3; i++)
      for (j = 0; j < 3; j++)
	for (k = 0; k < 3; k++) {
	  if ((E = mp_mul(&per[i][k], &per[k][j], &T1)) != MP_OKAY) {
	    return E;
	  }
	  if ((E = mp_add(&T1, &tmp[i][j], &tmp[i][j])) != MP_OKAY) {
	    return E;
	  }
	}
    for (i = 0; i < 3; i++)
      for (j = 0; j < 3; j++) {
	if ((E = mp_copy(&tmp[i][j], &per[i][j])) != MP_OKAY) {
	  return E;
	}
      }
    n /= 2;
  }
  if ((E = mp_copy(&ret[2][2], r)) != MP_OKAY) {
    return E;
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      mp_clear(&tmp[i][j]);
      mp_clear(&ret[i][j]);
      mp_clear(&per[i][j]);
    }
  }
  mp_clear_multi(&T1, NULL);
  return MP_OKAY;
}

#endif
