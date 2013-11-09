#include <tommath.h>
#ifdef BN_MP_STIRLING1_C

static mp_int **stirling1_mat = NULL;
static unsigned long stirling1_n = 0;
static unsigned long stirling1_m = 0;

/* Does all memory get free'd here? Really? ALL?  */
int mp_stirling1(unsigned long n, unsigned long m, mp_int * c)
{
  unsigned long i, j, k;
  mp_int temp;
  int e;

  if (n < m) {
    mp_set(c, 0);
    return MP_OKAY;
  }
  if (n == m) {
    mp_set(c, 1);
    return MP_OKAY;
  }
  if (m == 0 || n == 0) {
    mp_set(c, 0);
    return MP_OKAY;
  }
  /* Check if cached */
  if (stirling1_n >= n && stirling1_m >= m) {
    if ((e = mp_copy(&(stirling1_array)[n][m], c)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }
  if (m == 1) {
    /* odd n  */
    if ((n & 0x1)) {
      if ((e = mp_factorial(n - 1, c)) != MP_OKAY) {
	return e;
      }
      return MP_OKAY;
    } else {
      if ((e = mp_factorial(n - 1, c)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_neg(c, c)) != MP_OKAY) {
	return e;
      }
      return MP_OKAY;
    }
  }
  if (m == n - 1) {
    if ((n & 0x1)) {
      if ((e = mp_binomial(n, 2, c)) != MP_OKAY) {
	return e;
      }
      return MP_OKAY;
    } else {
      if ((e = mp_binomial(n, 2, c)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_neg(c, c)) != MP_OKAY) {
	return e;
      }
      return MP_OKAY;
    }
  }
  /* There are more cut-offs possible e.g.: s1(n,n-3), s1(n,n-4)...  */

  /* Allocate memory for one row with m columns */
  stirling1_array = malloc(sizeof(mp_int *) * (n + 1));
  if (stirling1_array == NULL) {
    return MP_MEM;
  }
  stirling1_array[0] = malloc(sizeof(mp_int) * (m + 1));
  if (stirling1_array[0] == NULL) {
    return MP_MEM;
  }
  /* Initialize first row */
  for (i = 0; i <= m; i++) {
    if ((e = mp_init(&stirling1_array[0][i])) != MP_OKAY) {
      return e;
    }
  }
  if ((e = mp_init(&temp)) != MP_OKAY) {
    return e;
  }
  /* Starting point of recurrence */
  if ((e = mp_set(&(stirling1_array)[0][0], 1)) != MP_OKAY) {
    return e;
  }

  for (i = 1; i <= n; i++) {
    /* Allocate memory for next (result) row */
    stirling1_array[i] = malloc(sizeof(mp_int) * (m + 1));
    if (stirling1_array[i] == NULL) {
      return MP_MEM;
    }
    /* Intialize first big-int of row for the free[i-1][j-1] of next row */
    /* gets free'd in next loop */
    if ((e = mp_init(&stirling1_array[i][0])) != MP_OKAY) {
      return e;
    }
    for (j = 1; j <= m; j++) {
      /* Intialize big-int for the result */
      if ((e = mp_init(&stirling1_array[i][j])) != MP_OKAY) {
	return e;
      }
      if (j <= i) {
	//s(i, j) = s(i - 1, j - 1) - (i - 1) * s(i - 1, j)
	if ((e = mp_set_int(&temp, i - 1)) != MP_OKAY) {
	  return e;
	}
	if ((e = mp_mul(&(stirling1_array)[i - 1][j], &temp, &temp)) != MP_OKAY) {
	  return e;
	}
	if ((e =
	     mp_sub(&(stirling1_array)[i - 1][j - 1], &temp,
		    &(stirling1_array)[i][j])) != MP_OKAY) {
	  return e;
	}
	/* free memory of used big-ints */
	mp_clear(&(stirling1_array)[i - 1][j - 1]);
      } else {
	if ((e = mp_set(&(stirling1_array)[i][j], 0)) != MP_OKAY) {
	  return e;
	}
	/* Even if we haven't used it, we should free it */
	mp_clear(&(stirling1_array)[i - 1][j - 1]);
      }
    }
    /* free memory of last element of last row */
    mp_clear(&(stirling1_array)[i - 1][j - 1]);
    /* free memory of used row. All allocated/free'd memory must balance
     * out here, with the exeception of the last row */
    free(stirling1_array[i - 1]);
  }

  if ((e = mp_copy(&(stirling1_array)[n][m], c)) != MP_OKAY) {
    return e;
  }

  /* Clean up */
  mp_clear(&temp);
  /* Free last row */
  for (j = 0; j <= m; j++) {
    mp_clear(&(stirling1_array)[i - 1][j]);
  }
  free(stirling1_array[i - 1]);

  /* Every element (big-int) must have been free'd here, so free the array */
  free(stirling1_array);
  /* Set to NULL */
  stirling1_array = NULL;

  return MP_OKAY;
}
#endif
