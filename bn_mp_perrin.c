#include <tommath.h>
#ifdef BN_MP_PERRIN_C


int mp_perrin(unsigned long n, mp_int * r)
{
  int i, j, k;
  mp_int a, b, c, d, e, f, g, h, p;
  mp_int t1, t2, t3, t4, t5, t6, t7, t8, t9;
  mp_int r1, r2, r3, r4, r5, r6, r7, r8, r9;
  mp_int v1, v2, v3;
  mp_int T1, T2, T3;
  int E;
  mp_digit list[14] = { 3, 0, 2, 3, 2, 5, 5, 7, 10, 12, 17, 22, 29, 39 };
  if (n < 14) {
    if ((e = mp_set(r, list[n])) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }

  n -= 2;

  if ((e = mp_init_multi(&a, &b, &c, &d, &e, &f, &g, &h, &p,
			 &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9,
			 &r1, &r2, &r3, &r4, &r5, &r6, &r7, &r8, &r9,
			 &v1, &v2, &v3, &T1, &T2, &T3, NULL)) != MP_OKAY) {
    return e;
  }

  /*
   * The Padovan numbers P(n-3) are in ret[1][1] and ret[2][2] if one starts
   * with n and not with n-2
   * The Padovan numbers are defined differently in OEIS (with starting values
   * 1,0,0) and Wikipedia/Wolfram (with starting values 1,1,1).
   * The algorithm above gives the numbers according to OEIS
   * 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, 15, 16, 17, 18, 19, 20,...
   * 1, 0, 0, 1, 0, 1, 1, 1, 2, 2, 3, 4, 5, 7, 9, 12, 16, 21, 28, 37, 49,...
   * According to Wikipedia/Wolfram:
   * 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,...
   * 1, 1, 1, 2, 2, 3, 4, 5, 7, 9, 12, 16, 21, 28, 37, 49,...
   * 
   * So it is just a difference of 5 (five).
   */

  if ((e = mp_set(&a, 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&b, 1)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&c, 0)) != MP_OKAY) {
    return e;
  }

  if ((e = mp_set(&d, 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&e, 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&f, 1)) != MP_OKAY) {
    return e;
  }

  if ((e = mp_set(&g, 1)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&h, 1)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&p, 0)) != MP_OKAY) {
    return e;
  }


  if ((e = mp_set(&r1, 1)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&r2, 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&r3, 0)) != MP_OKAY) {
    return e;
  }

  if ((e = mp_set(&r4, 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&r5, 1)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&r6, 0)) != MP_OKAY) {
    return e;
  }

  if ((e = mp_set(&r7, 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&r8, 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&r9, 1)) != MP_OKAY) {
    return e;
  }

  if ((e = mp_set(&v1, 3)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&v2, 0)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&v3, 2)) != MP_OKAY) {
    return e;
  }

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
  mp_int vec[3] = { v1, v2, v3 };

  while (n) {
    if (n & 1) {
      for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++) {
	  if ((e = mp_set(&tmp[i][j], 0)) != MP_OKAY) {
	    return e;
	  }
	}
      for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++)
	  for (k = 0; k < 3; k++) {
	    if ((e = mp_mul(&ret[i][k], &per[k][j], &T1)) != MP_OKAY) {
	      return e;
	    }
	    if ((e = mp_add(&T1, &tmp[i][j], &tmp[i][j])) != MP_OKAY) {
	      return e;
	    }
	  }
      for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++) {
	  if ((e = mp_copy(&tmp[i][j], &ret[i][j])) != MP_OKAY) {
	    return e;
	  }
	}
    }
    for (i = 0; i < 3; i++)
      for (j = 0; j < 3; j++) {
	if ((e = mp_set(&tmp[i][j], 0)) != MP_OKAY) {
	  return e;
	}
      }
    for (i = 0; i < 3; i++)
      for (j = 0; j < 3; j++)
	for (k = 0; k < 3; k++) {
	  if ((e = mp_mul(&per[i][k], &per[k][j], &T1)) != MP_OKAY) {
	    return e;
	  }
	  if ((e = mp_add(&T1, &tmp[i][j], &tmp[i][j])) != MP_OKAY) {
	    return e;
	  }
	}
    for (i = 0; i < 3; i++)
      for (j = 0; j < 3; j++) {
	if ((e = mp_copy(&tmp[i][j], &per[i][j])) != MP_OKAY) {
	  return e;
	}
      }
    n /= 2;
  }
  /* It is not necessayry to compute all, if we need only one */
  /*
   * for(i=0; i<3; i++){
   * This loop computes P(n), P(n+2), P(n+4)
   * Set T3 to zero here if you want consecutive Perrin numbers
   * for(j=0; j<3; j++){
   * mp_mul(&ret[i][j],&vec[j],&T2);
   * mp_add(&T2,&T3,&T3);
   * }
   * mp_copy(&T3,&tmp[i][0]);
   * }
   */
  /*
   * for(i=0; i<3; i++){
   * mp_fwrite(&tmp[i][0],10,stdout);
   * puts("");
   * }
   * mp_copy(&tmp[2][0],r);
   */
  /* It is also the trace of perrin(n) (same but without the subtraction of two
   * at the start of this function) */
  for (i = 0; i < 3; i++) {
    if ((e = mp_mul(&ret[2][i], &vec[i], &T2)) != MP_OKAY) {
      return e;
    }
    if ((e = mp_add(&T2, &T3, &T3)) != MP_OKAY) {
      return e;
    }
  }
  if ((e = mp_copy(&T3, r)) != MP_OKAY) {
    return e;
  }
  /*
   * The matrix handling seems to mess up some pointers. If I try it with
   * mp_clear_multi() I get a SIGABRT with a "nmap_chunk(): invalid pointer".
   */
  for (i = 0; i < 3; i++) {
    mp_clear(&vec[i]);
    for (j = 0; j < 3; j++) {
      mp_clear(&tmp[i][j]);
      mp_clear(&ret[i][j]);
      mp_clear(&per[i][j]);
    }
  }
  mp_clear_multi(&T1, &T2, &T3, NULL);
  return MP_OKAY;
}


#endif
