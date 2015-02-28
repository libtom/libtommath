#include <tommath.h>
#ifdef BN_MP_FPUT_C


int mp_fput(mp_int * a, int base, FILE *stream)
{
    char *str, *s0;
    size_t size, n;
    int err;

    size = mp_digits(a, base) + 10;
    str = malloc(size * sizeof(char));
    if (NULL == str) {
#ifdef DEBUG
	fprintf(stderr, "malloc failed to allocate %d bytes\n",
		size * sizeof(char));
#endif
	return MP_MEM;
    }
    s0 = str;
    n = size;
    while (--n) {
	*s0++ = '\0';
    }
    if ((err = mp_toradix(a, str, base)) != MP_OKAY) {
	return err;
    }
    fputs(str,stream);
    // This library might get used for cryptographic purposes, so
    // overwrite the memory, at least.
    n = size;
    s0 = str;
    while (--n) {
	*s0++ = '\0';
    }
    free(str);
    return MP_OKAY;
}

#endif
