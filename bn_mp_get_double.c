#include <tommath.h>
#ifdef BN_MP_GET_DOUBLE_C


#include <math.h>
#if __STDC_VERSION__ >= 199901L
    #include <fenv.h>
#endif
#include <float.h>
/* Convert to double, assumes IEEE-754 conforming double. From code by 
   gerdr (Gerhard R.) https://github.com/gerdr */
int mp_get_double(mp_int * a, double *d)
{
    int digits_needed, i, limit;
    double multiplier;

    /* digits_needed * DIGIT_BIT >= 64 bit */
    digits_needed = ( (DBL_MANT_DIG + DIGIT_BIT) / DIGIT_BIT) + 1;

    multiplier = (double) (MP_MASK + 1);

    *d = 0.0;

    /* Could be assumed, couldn't it? */
    mp_clamp(a);

    i = a->used;

    limit = (i <= digits_needed) ? 0 : i - digits_needed;

    while (i-- > limit) {
	*d += a->dp[i];
	*d *= multiplier;
    }

    if (a->sign == MP_NEG) {
	*d *= -1.0;
    }

    *d *= pow(2.0, i * DIGIT_BIT);

    /* Handle overflow */
    if (*d == HUGE_VAL) {
#if __STDC_VERSION__ >= 199901L
	feraiseexcept(FE_OVERFLOW);
#endif
	return MP_RANGE;
    }

    return MP_OKAY;
}

#endif
