#include "tommath_private.h"
#ifdef S_MP_RADIX_MAP_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* chars used in radix conversions */
const char s_mp_radix_map[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
const uint8_t s_mp_radix_map_reverse[] = {
   0x3e, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x01, 0x02, 0x03, 0x04, /* +,-./01234 */
   0x05, 0x06, 0x07, 0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, /* 56789:;<=> */
   0xff, 0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, /* ?@ABCDEFGH */
   0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, /* IJKLMNOPQR */
   0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0xff, 0xff, /* STUVWXYZ[\ */
   0xff, 0xff, 0xff, 0xff, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, /* ]^_`abcdef */
   0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, /* ghijklmnop */
   0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d  /* qrstuvwxyz */
};

MP_STATIC_ASSERT(correct_radix_map_reverse_size, sizeof(s_mp_radix_map_reverse) == MP_RADIX_MAP_REVERSE_SIZE)

/* Exponents chosen such that b^(y) <= 2^20 */
const uint8_t s_mp_radix_exponent_y[] = {  0, 0,                      /*  0 .. 1*/
                                           20, 12, 10, 8, 7, 7, 6, 6, /*  2 .. 9 */
                                           6, 5, 5, 5, 5, 5, 5, 4,    /* 10 .. 17 */
                                           4, 4, 4, 4, 4, 4, 4, 4,    /* 18 .. 25 */
                                           4, 4, 4, 4, 4, 4, 4, 3,    /* 26 .. 33 */
                                           3, 3, 3, 3, 3, 3, 3, 3,    /* 34 .. 41 */
                                           3, 3, 3, 3, 3, 3, 3, 3,    /* 42 .. 49 */
                                           3, 3, 3, 3, 3, 3, 3, 3,    /* 51 .. 57 */
                                           3, 3, 3, 3, 3, 3, 3        /* 58 .. 64 */
                                        };

MP_STATIC_ASSERT(correct_mp_radix_exponent_y, sizeof(s_mp_radix_exponent_y) == MP_RADIX_EXPONENT_Y_SIZE)

#endif
