#include <string.h>
#include <stdlib.h>
#include <time.h>

/*
 * Configuration
 */
#ifndef LTM_DEMO_TEST_REDUCE_2K_L
/* This test takes a moment so we disable it by default, but it can be:
 * 0 to disable testing
 * 1 to make the test with P = 2^1024 - 0x2A434 B9FDEC95 D8F9D550 FFFFFFFF FFFFFFFF
 * 2 to make the test with P = 2^2048 - 0x1 00000000 00000000 00000000 00000000 4945DDBF 8EA2A91D 5776399B B83E188F
 */
#define LTM_DEMO_TEST_REDUCE_2K_L 0
#endif

#include "tommath_private.h"


#define EXPECT(a) do { if (!(a)) { fprintf(stderr, "%s, line %d: EXPECT(%s) failed\n", __func__, __LINE__, #a); goto LBL_ERR; } } while(0)
#define DO_WHAT(a, what) do { mp_err err; if ((err = (a)) != MP_OKAY) { fprintf(stderr, "%s, line %d: DO(%s) failed: %s\n", __func__, __LINE__, #a, mp_error_to_string(err)); what; } } while(0)
#define DO(a) DO_WHAT(a, goto LBL_ERR)
#define DOR(a) DO_WHAT(a, return EXIT_FAILURE)


extern void ndraw(const mp_int* a, const char* name);
extern void print_header(void);
