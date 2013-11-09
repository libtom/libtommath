/*
                  k
                 ====                 /   \
             1   \      n      k - j  | k |
   S(n,k) =  --   >    j  (- 1)       |   |
             k!  /                    | j |
                 ====                 \   /
                j = 0

  The algorithm is based on building consecutive binomials
  and the symmetry of them, such that only the last power
  of the sum (the j^n) needs to be computed.

  Compute k-consecutive binomials binom(n,k) easily
  with the help of binom(n,k) = fallingfactorial(n,k)/k!
  or, to your taste, binom(n,k) = n!/(k!*(n-k)!).

  More optimzations: One can e.g. precompute the binomials for reuse
  which saves half of the calculations.
    -> precompute binom(n,k) for 0 < k < ceil(k/2) -> binom[]
    -> loop j over range 1, k by steps of 2 
    -> use binom[j] and  binom[k-j] respectively
  Better (replaces some powers with shifts)
    -> precompute binom(n,k) for 0 < k < ceil(k/2) -> binom[]
    -> loop j over range 1, k by steps of 2
      -> set J = j
      -> compute u = j^n
      -> loop J over range 1, k  
        -> use binom[J] and  binom[k-J] respectively
        -> shift u left by n
        -> set J = 2*J
       
  The author of this code found not much difference for smaller
  values of S(n,m), mainly for reasons of locality. YMMV.

  An independent implementation by Fredrik Johansson of the second algo. from
  above can be found in the FLINT library at
    http://www.flintlib.org/
  Fredrik's implementation is also guilty for causing the
  author to rewrite his convoluted mess (worse than the LISP
  version in MAXIMA) to make it legible, although it is a
  (negligible) bit slower now.

  As a final note: it is not necessary to compute any power of two in the
  calculation of the binomials, they can be added last with a simple shift.
  But stripping them is overhead and adding them is overhead, too, so the
  resulting numbers need to get large enough to make it useful.
*/
int mp_stirling2(unsigned long n, unsigned long k, mp_int * c)
{
  mp_int t, jot, sum, temp, enn;
  unsigned long j;
  int e;

  if (n < k || k == 0) {
    if ((e = mp_set(c, 0)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }
  if (k == 1) {
    if ((e = mp_set(c, 1)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }
  if (k == 2) {
    if (n >= INT_MAX) {
      if ((e = mp_init_multi(&temp, &enn, NULL)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_set_int(&enn, n - 1)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_set(&temp, 2)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_expt(&temp, &enn, c)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_sub(c, 1, c)) != MP_OKAY) {
	return e;
      }
      mp_clear_multi(&temp, &enn, NULL);
      return MP_OKAY;
    } else {
      if ((e = mp_set(c, 1)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_expt(c, n - 1, c)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_sub(c, 1, c)) != MP_OKAY) {
	return e;
      }
      return MP_OKAY;
    }
    return MP_OKAY;
  }
  if ((n - k) == 1) {
    if ((e = mp_biomial(n, 2)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }

  if ((e = mp_init_multi(&t, &jot, &sum, &temp, &enn, NULL)) != MP_OKAY) {
    return e;
  }

  if ((e = mp_set(&t, 1)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_set(&sum, 0)) != MP_OKAY) {
    return e;
  }

  if ((e = mp_set_int(&enn, n)) != MP_OKAY) {
    return e;
  }
  /* binom(n,0) = 1 */
  for (j = 1; j <= (k / 2); j++) {
    if (k + 1 - j >= 1 << DIGIT_BIT) {
      if ((e = mp_set_int(&temp, k + 1 - j)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_mul(&t, &temp, &t)) != MP_OKAY) {
	return e;
      }
    } else {
      if ((e = mp_mul_d(&t, k + 1 - j, &t)) != MP_OKAY) {
	return e;
      }
    }
    if (j >= 1 << DIGIT_BIT) {
      if ((e = mp_set_int(&temp, j)) != MP_OKAY) {
	return e;
      }
      if ((e = mp_div(&t, &temp, &t, NULL)) != MP_OKAY) {
	return e;
      }
    } else {
      if ((e = mp_div_d(&t, j, &t, NULL)) != MP_OKAY) {
	return e;
      }
    }
    if ((e = mp_set_int(&jot, j)) != MP_OKAY) {
      return e;
    }
    if (n >= 1 << DIGIT_BIT) {
      if ((e = mp_expt(&jot, &enn, &jot)) != MP_OKAY) {
	return e;
      }
    } else {
      if ((e = mp_expt_d(&jot, n, &jot)) != MP_OKAY) {
	return e;
      }
    }

    if ((e = mp_mul(&t, &jot, &temp)) != MP_OKAY) {
      return e;
    }
    if ((k + j) & 0x1) {
      if ((e = mp_sub(&sum, &temp, &sum)) != MP_OKAY) {
	return e;
      }
    } else {
      if ((e = mp_add(&sum, &temp, &sum)) != MP_OKAY) {
	return e;
      }
    }
    /* The middle one is, well, the single middle one */
    if ((j << 1) ^ k) {
      if ((e = mp_set_int(&jot, k - j)) != MP_OKAY) {
	return e;
      }
      if (n >= 1 << DIGIT_BIT) {
	if ((e = mp_expt(&jot, &enn, &jot)) != MP_OKAY) {
	  return e;
	}
      } else {
	mif((e = p_expt_d(&jot, n, &jot)) != MP_OKAY) {
	  return e;
	}
      }
      if ((e = mp_mul(&t, &jot, &temp)) != MP_OKAY) {
	return e;
      }
      if (j & 0x1) {
	if ((e = mp_sub(&sum, &temp, &sum)) != MP_OKAY) {
	  return e;
	}
      } else {
	if ((e = mp_add(&sum, &temp, &sum)) != MP_OKAY) {
	  return e;
	}
      }
    }
  }
  /* Last round with last power */
  if ((e = mp_set_int(&jot, k)) != MP_OKAY) {
    return e;
  }
  if (n >= 1 << DIGIT_BIT) {
    if ((e = mp_expt(&jot, &enn, &jot)) != MP_OKAY) {
      return e;
    }
  } else {
    if ((e = mp_expt_d(&jot, n, &jot)) != MP_OKAY) {
      return e;
    }
  }

  if ((e = mp_add(&sum, &jot, &sum)) != MP_OKAY) {
    return e;
  }
  /* divide by k! */
  if ((e = mp_factorial(k, &t)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_div(&sum, &t, &sum, NULL)) != MP_OKAY) {
    return e;
  }
  /* Return result */
  mp_exch(&sum, c);
  /* Clean up */
  mp_clear_multi(&t, &jot, &sum, &temp, &enn, NULL);
  return MP_OKAY;
}
