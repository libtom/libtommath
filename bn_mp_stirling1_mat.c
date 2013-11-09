
static mp_int **stirling1_mat = NULL;
static unsigned long stirling1_n = 0;
static unsigned long stirling1_m = 0;

/*
    Algorithm 3.17,
    Donald Kreher and Douglas Simpson,
    Combinatorial Algorithms,
    CRC Press, 1998, page 89.
*/
int mp_stirling1_mat(unsigned long n, unsigned long m, mp_int * c)
{
  unsigned long i, j, k;
  mp_int temp;
  int e;

  /* We assume the caller knows what it does and return only in case of error */
  if (n < m) {
    return MP_VAL;
  }
  /* Not necessarily an error? */
  if (m == 0 || n == 0) {
    return MP_VAL;
  }

  /* Check if already cached */
  if (stirling1_n >= n && stirling1_m >= m) {
    if ((e = mp_copy(&(stirling1_mat)[n][m], c)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }

  /* TODO: check if array already exists and reallocate but for */
  /*       now, we just free the memory and start from scratch */
  if (stirling1_mat != NULL) {
    for (k = 0; k <= stirling1_n; k++) {
      for (i = 0; i <= stirling1_m; i++) {
	mp_clear(&stirling1_mat[k][i]);
      }
      free(stirling1_mat[k]);
    }
    free(stirling1_mat);
    stirling1_mat = NULL;
    stirling1_n = 0;
    stirling1_m = 0;
  }
  /* Allocate memory for n rows with m columns */
  /* Be aware that it will allocate a lot of memory, a quarter of a GiByte even
   * for some quite moderate s1(1000,500) */
  stirling1_mat = malloc(sizeof(mp_int *) * (n + 1));
  if (stirling1_mat == NULL) {
    return MP_MEM;
  }
  for (k = 0; k <= n; k++) {
    stirling1_mat[k] = malloc(sizeof(mp_int) * (m + 1));
    if (stirling1_mat[k] == NULL) {
      return MP_MEM;
    }
    for (i = 0; i <= m; i++) {
      if ((e = mp_init(&stirling1_mat[k][i])) != MP_OKAY) {
	return e;
      }
    }
  }
  if ((e = mp_init(&temp)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&(stirling1_mat)[0][0], 1)) != MP_OKAY) {
    return e;
  }

  for (i = 1; i <= n; i++) {
    for (j = 1; j <= m; j++) {
      if (j <= i) {
	//s(i, j) = s(i - 1, j - 1) - (i - 1) * s(i - 1, j)
	if ((e = mp_set_int(&temp, i - 1)) != MP_OKAY) {
	  return e;
	}
	if ((e = mp_mul(&(stirling1_mat)[i - 1][j], &temp, &temp)) != MP_OKAY) {
	  return e;
	}
	if ((e =
	     mp_sub(&(stirling1_mat)[i - 1][j - 1], &temp,
		    &(stirling1_mat)[i][j])) != MP_OKAY) {
	  return e;
	}
      } else {
	if ((e = mp_set(&(stirling1_mat)[i][j], 0)) != MP_OKAY) {
	  return e;
	}
      }
    }
  }

  stirling1_n = n;
  stirling1_m = m;

  if ((e = mp_copy(&(stirling1_mat)[n][m], c)) != MP_OKAY) {
    return e;
  }

  mp_clear(&temp);
  return MP_OKAY;
}

void mp_stirling1_free(void)
{
  unsigned long i, k;
  if (stirling1_mat != NULL) {
    for (k = 0; k <= stirling1_n; k++) {
      for (i = 0; i <= stirling1_m; i++) {
	mp_clear(&stirling1_mat[k][i]);
      }
      free(stirling1_mat[k]);
    }
    free(stirling1_mat);
    stirling1_mat = NULL;
  }
  stirling1_n = 0;
  stirling1_m = 0;
}
