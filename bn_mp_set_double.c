#include <tommath.h>
#ifdef BN_MP_SET_DOUBLE_C


#include <math.h>
#if __STDC_VERSION__ >= 199901L
    #include <fenv.h>
#endif
/* integer part of a double, rounding mode defaults to FE_TONEAREST */
int mp_set_double(mp_int * c, double d, int rounding_mode)
{
    int exp, res, sign;
    double frac, rnd;
#if __STDC_VERSION__ >= 199901L
    fenv_t envp;
#endif
    sign = (d < 0) ? MP_NEG : MP_ZPOS;
    mp_zero(c);
#if __STDC_VERSION__ >= 199901L
    if (isinf(d) || isnan(d)) {
	c->sign = sign;
	return MP_VAL;
    }

    /* backup of floating point environment settings, better safe than sorry */
    if (fegetenv(&envp)) {
	return MP_VAL;
    }
#endif

#ifndef FE_TONEAREST
    #define FE_TONEAREST 0
#endif
    if (rounding_mode <= 0) {
	rounding_mode = FE_TONEAREST;
    }
#if __STDC_VERSION__ >= 199901L
    if (fesetround(rounding_mode)) {
	return MP_VAL;
    }

    rnd = nearbyint(d);
#else
    rnd = d;
#endif

#if __STDC_VERSION__ >= 199901L
    if (fesetenv(&envp)) {
	return MP_VAL;
    }
#endif
    frac = frexp(abs(rnd), &exp);

    if (frac == 0) {
	c->sign = sign;
	return MP_OKAY;
    }

    while (exp-- >= 0) {
	frac *= 2.0;
	if (frac >= 1.0) {
	    if ((res = mp_add_d(c, 1, c)) != MP_OKAY) {
		return res;
	    }
	    frac -= 1.0;
	}
	if (exp > 0) {
	    if ((res = mp_mul_2d(c, 1, c)) != MP_OKAY) {
		return res;
	    }
	}
    }
    c->sign = sign;
    return MP_OKAY;
}

#endif
