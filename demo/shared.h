#include <string.h>
#include <time.h>

/*
 * Configuration
 */
#ifndef LTM_DEMO_TEST_VS_MTEST
#define LTM_DEMO_TEST_VS_MTEST 1
#endif

#ifndef LTM_DEMO_TEST_REDUCE_2K_L
/* This test takes a moment so we disable it by default, but it can be:
 * 0 to disable testing
 * 1 to make the test with P = 2^1024 - 0x2A434 B9FDEC95 D8F9D550 FFFFFFFF FFFFFFFF
 * 2 to make the test with P = 2^2048 - 0x1 00000000 00000000 00000000 00000000 4945DDBF 8EA2A91D 5776399B B83E188F
 */
#define LTM_DEMO_TEST_REDUCE_2K_L 0
#endif

#ifdef LTM_DEMO_REAL_RAND
#define LTM_DEMO_RAND_SEED  time(NULL)
#else
#define LTM_DEMO_RAND_SEED  23
#endif

#include "tommath.h"
