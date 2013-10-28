#include <tommath.h>
#ifdef BN_MP_ISPERFPOWER_C
/* 
  There exists mp_is_square() but it does not return the found root, only
  if it is a square, so we do our own.
  The author wants to stress that he _hates_ to do that, especially if the
  resulting code is about 99% the very same!
*/
/* Check if remainders are possible squares - fast exclude non-squares */
static const char rem_128[128] = {
    0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1
};

static const char rem_105[105] = {
    0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
    0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1
};
/* 
  Sets ret to nonzero value if arg is square, 0 if not
  Sets t to the square root of arg if one is available, 0 if not
 */
static int mp_issquare(mp_int * arg, int *ret, mp_int * t)
{
    int res;
    mp_digit c;
    mp_int tmp;

    unsigned long r;

    /* Default to Non-square :) */
    *ret = MP_NO;

    if (arg->sign == MP_NEG) {
        return MP_VAL;
    }

    /* digits used?  (TSD) */
    if (arg->used == 0) {
        return MP_OKAY;
    }

    /* First check mod 128 (suppose that DIGIT_BIT is at least 7) */
    if (rem_128[127 & DIGIT(arg, 0)] == 1) {
        mp_set_int(t, (mp_digit) (0));
        return MP_OKAY;
    }

    /* Next check mod 105 (3*5*7) */
    if ((res = mp_mod_d(arg, 105, &c)) != MP_OKAY) {
        mp_set_int(t, (mp_digit) (0));
        return res;
    }
    if (rem_105[c] == 1) {
        mp_set_int(t, (mp_digit) (0));
        return MP_OKAY;
    }
    if ((res =
         mp_init_set_int(t,
                         11L * 13L * 17L * 19L * 23L * 29L * 31L)) != MP_OKAY) {
        mp_set_int(t, (mp_digit) (0));
        return res;
    }
    if ((res = mp_mod(arg, t, t)) != MP_OKAY) {
        goto ERR;
    }
    r = mp_get_int(t);
    /* Check for other prime modules. We know that res
     * is already equal to MP_OKAY from the mp_mod call 
     */
    if ((1L << (r % 11)) & 0x5C4L)
        goto ERR;
    if ((1L << (r % 13)) & 0x9E4L)
        goto ERR;
    if ((1L << (r % 17)) & 0x5CE8L)
        goto ERR;
    if ((1L << (r % 19)) & 0x4F50CL)
        goto ERR;
    if ((1L << (r % 23)) & 0x7ACCA0L)
        goto ERR;
    if ((1L << (r % 29)) & 0xC2EDD0CL)
        goto ERR;
    if ((1L << (r % 31)) & 0x6DE2B848L)
        goto ERR;

    /* Final check - is sqr(sqrt(arg)) == arg ? */
    if ((res = mp_sqrt(arg, t)) != MP_OKAY) {
        goto ERR;
    }
    mp_init(&tmp);
    if ((res = mp_sqr(t, &tmp)) != MP_OKAY) {
        goto ERR;
    }

    *ret = (mp_cmp_mag(&tmp, arg) == MP_EQ) ? MP_YES : MP_NO;
    mp_clear(&tmp);
    return res;
  ERR:
    mp_set_int(t, (mp_digit) (0));
    mp_clear(&tmp);
    return res;
}

int mp_isperfpower(mp_int * z, mp_int * rootout, mp_int * exponent)
{
    mp_int root, power, tempr;
    mp_bitset_t *bst;
    unsigned long p, max, highbit, lowbit;
    int ret, e;
    p = 2;
    /* Place some sanity checks here */
    /* No, really! Do it! */

    /* ceil(log_2(n)) */
    max = (unsigned long) mp_highbit(z);
    highbit = max;
    lowbit = (unsigned long) mp_lowbit(z);

    /* Is it N=2^p ? */
    if (highbit == lowbit) {
        if (exponent != NULL) {
            if ((e = mp_set_int(exponent, (mp_digit) highbit)) != MP_OKAY) {
                return e;
            }
        }
        if (rootout != NULL) {
            if ((e = mp_set_int(rootout, (mp_digit) (2))) != MP_OKAY) {
                return e;
            }
        }
        return MP_YES;
    }
    /* mp_is[_]square() is not much faster, but still a bit */
    if (rootout != NULL) {
        if ((e = mp_issquare(z, &ret, rootout)) != MP_OKAY) {
            return e;
        }
    } else {
        if ((e = mp_is_square(z, &ret)) != MP_OKAY) {
            return e;
        }
    }
    if (ret != 0) {
        if (exponent != NULL) {
            if ((e = mp_set_int(exponent, (mp_digit) (2))) != MP_OKAY) {
                return e;
            }
        }
        return MP_YES;
    }

    if ((e = mp_init_multi(&root, &power, &tempr, NULL)) != MP_OKAY) {
        return e;
    }

    /* find perfection in higher powers */
    bst = malloc(sizeof(mp_bitset_t));
    if (bst == NULL) {
        return MP_MEM;
    }
    mp_bitset_alloc(bst, max);
    /* 
       libtommath contains a small table (ltm_prime_tab) with the first 256
       primes, the largest is therefore 1619 which is not enough, sadly.
       The table for is_small_prime() has the primes up to 2^16 which is enough
       for more than 41,000 decimal digits. Sounds a lot but is it sufficient to
       code a branch?
       A sieve of that size would easily fit into the L1 processor cache and
       might fit into the cache of an embedded processor. The table is quite
       large and might fit into the cache of a modern server processor but
       it is doubtlful if it fits in the cache of an embedded cpu.
       That would result in a lot of preprocessor branches.
       BTW: it is nigh impossible to get specifications from the manufactoring
       companies, they are quite mute about their L* caches.
     */
    mp_eratosthenes(bst);

    while ((p = mp_bitset_nextset(bst, p + 1)) < max) {
        if ((e = mp_n_root(z, p, &root)) != MP_OKAY) {
            return e;
        }
        if ((e = mp_copy(&root, &tempr)) != MP_OKAY) {
            return e;
        }
        if ((e = mp_expt_d(&root, p, &power)) != MP_OKAY) {
            return e;
        }
        if (mp_cmp(z, &power) == MP_EQ) {
            if (rootout != NULL) {
                if ((e = mp_copy(&tempr, rootout)) != MP_OKAY) {
                    return e;
                }
            }
            if (exponent != NULL) {
                if ((e = mp_set_int(exponent, (mp_digit) (p))) != MP_OKAY) {
                    return e;
                }
            }
            mp_clear_multi(&tempr, &root, &power,NULL);
            mp_bitset_free(bst);
            return MP_YES;
        }
    }
    if (rootout != NULL) {
        if ((e = mp_set_int(rootout, (mp_digit) (0))) != MP_OKAY) {
            return e;
        }
    }
    if (exponent != NULL) {
        if ((e = mp_set_int(exponent, (mp_digit) (0))) != MP_OKAY) {
            return e;
        }
    }
    mp_clear_multi(&root, &power,NULL);
    mp_bitset_free(bst);
    return MP_NO;
}

#endif

