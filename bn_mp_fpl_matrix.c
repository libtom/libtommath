#include <tommath.h>
#ifdef BN_MP_FPL_MATRIX_C
/*
  e.g.:
  n, 1,1,1,0 , 1,0,0,1, &c -> Fibonacci number n in c
  n, 1,1,1,0 , 1,2,2,1, &c -> Lucas number n in c
  n, 2,1,1,0 , 1,0,0,1, &c -> Pell number n in c
  n, 2,1,1,0 , 2,2,2,2, &c -> Pell-Lucas number n in c
  n, 2,1,1,0 , 1,1,1,1, &c -> Modified Pell number n in c
*/

int mp_fpl_matrix(unsigned long n, int m11, int m12, int m21, int m22,
		  int M11, int M12, int M21, int M22, mp_int * r)
{
  int i, j, k;
  mp_int a, b, c, d, e, f, g, h, t1, t2, t3, t4, t5;
  int E;

  E = MP_OKAY;

  if ((E =
       mp_init_multi(&a, &b, &c, &d, &e, &f, &g, &h, &t1, &t2, &t3, &t4, &t5,
		     NULL)) != MP_OKAY) {
    return E;
  }
  mp_set(&a, m11);
  mp_set(&b, m12);
  mp_set(&c, m21);
  mp_set(&d, m22);
  mp_set(&e, M11);
  mp_set(&f, M12);
  mp_set(&g, M21);
  mp_set(&h, M22);


  mp_int pell[2][2] = { {a, b}  , {c, d}  };
  mp_int ret[2][2] =  { {e, f}  , {g, h}  };
  mp_int tmp[2][2] =  { {t1, t2}, {t3, t4}};

  while (n) {
    if (n & 1) {
      for (i = 0; i < 2; i++)
	for (j = 0; j < 2; j++) {
	  mp_set(&tmp[i][j], 0);
	}
      for (i = 0; i < 2; i++)
	for (j = 0; j < 2; j++)
	  for (k = 0; k < 2; k++) {
	    if ((E = mp_mul(&ret[i][k], &pell[k][j], &t5)) != MP_OKAY) {
	      goto __ERR;
	    }
	    if ((E = mp_add(&t5, &tmp[i][j], &tmp[i][j])) != MP_OKAY) {
	      goto __ERR;
	    }
	  }
      for (i = 0; i < 2; i++)
	for (j = 0; j < 2; j++) {
	  if ((E = mp_copy(&tmp[i][j], &ret[i][j])) != MP_OKAY) {
	    goto __ERR;
	  }
	}
    }
    for (i = 0; i < 2; i++)
      for (j = 0; j < 2; j++) {
	mp_set(&tmp[i][j], 0);
      }
    for (i = 0; i < 2; i++)
      for (j = 0; j < 2; j++)
	for (k = 0; k < 2; k++) {
	  if ((E = mp_mul(&pell[i][k], &pell[k][j], &t5)) != MP_OKAY) {
	    goto __ERR;
	  }
	  if ((E = mp_add(&t5, &tmp[i][j], &tmp[i][j])) != MP_OKAY) {
	    goto __ERR;
	  }
	}
    for (i = 0; i < 2; i++)
      for (j = 0; j < 2; j++) {
	if ((E = mp_copy(&tmp[i][j], &pell[i][j])) != MP_OKAY) {
	  goto __ERR;
	}
      }
    n /= 2;
  }
  if ((E = mp_copy(&ret[0][1], r)) != MP_OKAY) {
    goto __ERR;
  }

__ERR:
  for (i = 0; i < 2; i++)
    for (j = 0; j < 2; j++) {
      mp_clear(&tmp[i][j]);
      mp_clear(&ret[i][j]);
      mp_clear(&pell[i][j]);
    }
  mp_clear(&t5);
  return E;
}
#endif

