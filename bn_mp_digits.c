#include <tommath.h>
#ifdef BN_MP_DIGITS_C

#include <math.h>

/*
 Returns the number of digits of base "base" of an mp_int.
 This is inexact and should not get used outside of calculating memory needs!
 (e.g: both of the decimal numbers 9 and 10 need 4 bits)
*/

long mp_digits(mp_int * a, int base)
{
    double log210;
    long bits = mp_count_bits(a);
    switch (base) {
    case 2:
	log210 = 1.0;
	break;
    case 8:
	log210 = 3.0;
	break;
    case 10:
	log210 = 3.321928094887362347870319429489390175865;
	break;
    case 16:
	log210 = 4.0;
	break;
    default:
	log210 = log(bits) / log(2.0);
	break;
    }
    return (long) (floor(bits / log210));
}
#endif
