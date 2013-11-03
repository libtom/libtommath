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

int mp_fpl_matrix(unsigned long n, int m11,int m12, int m21,int m22,
                                   int M11,int M12, int M21,int M22, mp_int * r)
{
  int i, j, k;
  mp_int a, b, c, d, e, f, g, h, t1, t2, t3, t4, t5;
  int E;
  if ((E =
       mp_init_multi(&a, &b, &c, &d, &e, &f, &g, &h, &t1, &t2, &t3, &t4, &t5,
		     NULL)) != MP_OKAY) {
    return E;
  }

  if ((E = mp_set(&a, m11)) != MP_OKAY) {
    return E;
  }
  if ((E = mp_set(&b, m12)) != MP_OKAY) {
    return E;
  }
  if ((E = mp_set(&c, m21)) != MP_OKAY) {
    return E;
  }
  if ((E = mp_set(&d, m22)) != MP_OKAY) {
    return E;
  }

  if ((E = mp_set(&e, M11)) != MP_OKAY) {
    return E;
  }
  if ((E = mp_set(&f, M12)) != MP_OKAY) {
    return E;
  }
  if ((E = mp_set(&g, M21)) != MP_OKAY) {
    return E;
  }
  if ((E = mp_set(&h, M22)) != MP_OKAY) {
    return E;
  }

  mp_int pell[2][2] = { {a, b} , {c, d} };
  mp_int ret[2][2]  = { {e, f} , {g, h} };
  mp_int tmp[2][2] = { {t1, t2} , {t3, t4} };

  while (n) {
    if (n & 1) {
      for (i = 0; i < 2; i++)
	for (j = 0; j < 2; j++) {
	  if ((E = mp_set(&tmp[i][j], 0)) != MP_OKAY) {
	    return E;
	  }
	}
      for (i = 0; i < 2; i++)
	for (j = 0; j < 2; j++)
	  for (k = 0; k < 2; k++) {
	    if ((E = mp_mul(&ret[i][k], &pell[k][j], &t5)) != MP_OKAY) {
	      return E;
	    }
	    if ((E = mp_add(&t5, &tmp[i][j], &tmp[i][j])) != MP_OKAY) {
	      return E;
	    }
	  }
      for (i = 0; i < 2; i++)
	for (j = 0; j < 2; j++) {
	  if ((E = mp_copy(&tmp[i][j], &ret[i][j])) != MP_OKAY) {
	    return E;
	  }
	}
    }
    for (i = 0; i < 2; i++)
      for (j = 0; j < 2; j++) {
	if ((E = mp_set(&tmp[i][j], 0)) != MP_OKAY) {
	  return E;
	}
      }
    for (i = 0; i < 2; i++)
      for (j = 0; j < 2; j++)
	for (k = 0; k < 2; k++) {
	  if ((E = mp_mul(&pell[i][k], &pell[k][j], &t5)) != MP_OKAY) {
	    return E;
	  }
	  if ((E = mp_add(&t5, &tmp[i][j], &tmp[i][j])) != MP_OKAY) {
	    return E;
	  }
	}
    for (i = 0; i < 2; i++)
      for (j = 0; j < 2; j++) {
	if ((E = mp_copy(&tmp[i][j], &pell[i][j])) != MP_OKAY) {
	  return E;
	}
      }
    n /= 2;
  }
  if ((E = mp_copy(&ret[0][1], r)) != MP_OKAY) {
    return E;
  }
  /*
   * The matrix handling seems to mess up some pointers. If I try it with
   * mp_clear_multi() I get a SIGABRT with a "nmap_chunk(): invalid pointer".
   */
  for (i = 0; i < 2; i++)
    for (j = 0; j < 2; j++) {
      mp_clear(&tmp[i][j]);
      mp_clear(&ret[i][j]);
      mp_clear(&pell[i][j]);
    }
  return MP_OKAY;
}
#endif
