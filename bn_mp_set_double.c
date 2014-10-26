#include <tommath.h>
#include <math.h>
#include <fenv.h>
/* integer part of a double, rounding mode defaults to FE_TONEAREST */
int mp_set_double(mp_int * c, double d, int rounding_mode)
{
    int exp, res, sign, oldenv;
    double frac, rnd;
    fenv_t envp;

    sign = (d < 0) ? MP_NEG : MP_ZPOS;
    mp_zero(c);

    if (isinf(d) || isnan(d)) {
	c->sign = sign;
	return MP_VAL;
    }

    /* backup of floating point environment settings, better safe than sorry */
    if (fegetenv(&envp)) {
	return MP_VAL;
    }
    if (rounding_mode <= 0) {
	rounding_mode = FE_TONEAREST;
    }
    if (fesetround(rounding_mode)) {
	return MP_VAL;
    }

    rnd = nearbyint(d);

    if (fesetenv(&envp)) {
	return MP_VAL;
    }

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

