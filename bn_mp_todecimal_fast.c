#include "tommath_private.h"
#include <string.h>
#ifdef BN_MP_TODECIMAL_FAST_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* store a bignum as a decimal ASCII string */
mp_err mp_todecimal_fast_rec(mp_int *number, mp_int *nL, mp_int *shiftL, mp_int *mL, int index, int left, char **result) {
    mp_int q, nLq, r;
    mp_err err;

    if (index < 0) {
        char *next_piece = calloc(4, sizeof(char));
        int s_s = snprintf(next_piece, 4, left ? "%u" : "%03u", mp_get_u32(number));
        int r_s = strlen(*result);
        (*result) = realloc(*result, r_s + s_s + 2);
        strcat(*result, next_piece);
        return MP_OKAY;
    }

    if ((err = mp_init_multi(&q, &nLq, &r, NULL)) != MP_OKAY) {
        goto LBL_ERR;
    }
    if ((err = mp_mul(number, &mL[index], &q)) != MP_OKAY) {
        goto LBL_ERR;
    }
    if ((err = mp_div_2d(&q, mp_get_u32(&shiftL[index]), &q, NULL)) != MP_OKAY) {
        goto LBL_ERR;
    }

    if ((err = mp_mul(&nL[index], &q, &nLq)) != MP_OKAY) {
        goto LBL_ERR;
    }

    if ((err = mp_sub(number, &nLq, &r)) != MP_OKAY) {
        goto LBL_ERR;
    }

    if (mp_isneg(&r)) {
        if ((err = mp_sub_d(&q, 1, &q)) != MP_OKAY) {
            goto LBL_ERR;
        }
        if ((err = mp_add(&r, &nL[index], &r)) != MP_OKAY) {
            goto LBL_ERR;
        }
    }

    --index;
    if (left && mp_iszero(&q)) {
        if ((err = mp_todecimal_fast_rec(&r, nL, shiftL, mL, index, 1, result)) != MP_OKAY) {
            goto LBL_ERR;
        }
    } else {
        if ((err = mp_todecimal_fast_rec(&q, nL, shiftL, mL, index, left, result)) != MP_OKAY) {
            goto LBL_ERR;
        }
        if ((err = mp_todecimal_fast_rec(&r, nL, shiftL, mL, index, 0, result)) != MP_OKAY) {
            goto LBL_ERR;
        }
    }

    err = MP_OKAY;

LBL_ERR:mp_clear_multi (&q, &nLq, &r, NULL);
    return err;
}

mp_err mp_todecimal_fast(mp_int *number, char **result) {
    mp_int n, shift, M, M2, M22, M4, M44;
    mp_int *nL, *shiftL, *mL;
    mp_err err;
    int index = 1;

    if ((err = mp_init_multi(&M2, &M22, &M4, &M44, NULL)) != MP_OKAY) {
        goto LBL_ERR;
    }

    if (mp_isneg(number)) {
        if ((err = mp_neg(number, number)) != MP_OKAY) {
            goto LBL_ERR;
        }
        *result[0] = '-';
    }
    if ((err = mp_init_set(&n, 1000)) != MP_OKAY) {
        goto LBL_ERR;
    }

    nL = malloc(20 * sizeof(mp_int));
    if ((err = mp_init_copy(&nL[0], &n)) != MP_OKAY) {
        goto LBL_ERR;
    }

    if ((err = mp_init_set(&shift, 20)) != MP_OKAY) {
        goto LBL_ERR;
    }

    shiftL = malloc(20 * sizeof(mp_int));
    if ((err = mp_init_copy(&shiftL[0], &shift)) != MP_OKAY) {
        goto LBL_ERR;
    }

    /* (8 * 2**$shift) / $n rounded up */
    if ((err = mp_init_set(&M, 8389)) != MP_OKAY) {
        goto LBL_ERR;
    }

    /* $M / 8, rounded up */
    mL = malloc(20 * sizeof(mp_int));
    if ((err = mp_init_set(&mL[0], 1049)) != MP_OKAY) {
        goto LBL_ERR;
    }

    while (1) {
        if ((err = mp_sqr(&n, &n)) != MP_OKAY) {
            goto LBL_ERR;
        }
        if (mp_cmp(&n, number) == MP_GT) {
            break;
        }

        if ((err = mp_mul_2(&shift, &shift)) != MP_OKAY) {
            goto LBL_ERR;
        }

        /* The following is a Newton-Raphson step, to restore the invariant
         * that $M is (8 * 2**$shift) / $n, rounded up. */
        {
            if ((err = mp_sqr(&M, &M2)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_sqr(&M2, &M4)) != MP_OKAY) {
                goto LBL_ERR;
            }

            if ((err = mp_mul(&M4, &n, &M4)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_div_2d(&M4, mp_get_ul(&shift) + 6, &M4, NULL)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_mul_2(&M2, &M2)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_sub(&M4, &M2, &M4)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_add_d(&M4, 1, &M4)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_div_2d(&M4, 3, &M4, NULL)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_sub_d(&M4, 1, &M4)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_neg(&M4, &M)) != MP_OKAY) {
                goto LBL_ERR;
            }
        }

        if ((err = mp_init_copy(&nL[index], &n)) != MP_OKAY) {
            goto LBL_ERR;
        }
        if ((err = mp_init_copy(&shiftL[index], &shift)) != MP_OKAY) {
            goto LBL_ERR;
        }

        /* Divide by 8, round up */
        {
            if ((err = mp_add_d(&M4, 1, &M4)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_div_2d(&M4, 3, &M4, NULL)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_sub_d(&M4, 1, &M4)) != MP_OKAY) {
                goto LBL_ERR;
            }
            if ((err = mp_neg(&M4, &M4)) != MP_OKAY) {
                goto LBL_ERR;
            }
        }
        if ((err = mp_init_copy(&mL[index], &M4)) != MP_OKAY) {
            goto LBL_ERR;
        }
        index++;
    }

    if ((err = mp_todecimal_fast_rec(number, nL, shiftL, mL, index - 1, 1, result)) != MP_OKAY) {
        goto LBL_ERR;
    }

    err = MP_OKAY;

LBL_ERR:mp_clear_multi (&n, &shift, &M, &M2, &M22, &M4, &M44, NULL);
    return err;
}

#endif
