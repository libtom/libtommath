#include "tommath_private.h"

#ifdef BN_CONVERSION_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#define MP_SET_UNSIGNED(name, w)                                        \
    void name(mp_int * a, uint##w##_t b)                                \
    {                                                                   \
        int i = 0;                                                      \
        while (b != 0u) {                                               \
            a->dp[i++] = ((mp_digit)b & MP_MASK);                       \
            if (w <= MP_DIGIT_BIT) { break; }                           \
            b >>= ((w <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);             \
        }                                                               \
        a->used = i;                                                    \
        a->sign = MP_ZPOS;                                              \
        MP_ZERO_DIGITS(a->dp + a->used, a->alloc - a->used);            \
    }
#define MP_SET_SIGNED(name, uname, w)                                   \
    void name(mp_int * a, int##w##_t b)                                 \
    {                                                                   \
        uname(a, (b < 0) ? -(uint##w##_t)b : (uint##w##_t)b);           \
        if (b < 0) { a->sign = MP_NEG; }                                \
    }
#define MP_INIT_INT(name , set, type)                     \
    mp_err name(mp_int * a, type b)                      \
    {                                                    \
        mp_err err;                                      \
        if ((err = mp_init(a)) != MP_OKAY) {             \
            return err;                                  \
        }                                                \
        set(a, b);                                       \
        return MP_OKAY;                                  \
    }
#define MP_GET_MAG(name, w)                                             \
    uint##w##_t name(const mp_int* a)                                   \
    {                                                                   \
        unsigned i = MP_MIN((unsigned)a->used, (unsigned)((w + MP_DIGIT_BIT - 1) / MP_DIGIT_BIT)); \
        uint##w##_t res = 0u;                                           \
        while (i --> 0u) {                                              \
            res <<= ((w <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);           \
            res |= (uint##w##_t)a->dp[i];                               \
            if (w <= MP_DIGIT_BIT) { break; }                           \
        }                                                               \
        return res;                                                     \
    }
#define MP_GET_SIGNED(name, mag, w)                                     \
    int##w##_t name(const mp_int* a)                                    \
    {                                                                   \
        uint64_t res = mag(a);                                          \
        return (a->sign == MP_NEG) ? (int##w##_t)-res : (int##w##_t)res;\
    }

#ifdef BN_MP_SET_U32_C
MP_SET_UNSIGNED(mp_set_u32, 32)
#endif

#ifdef BN_MP_SET_U64_C
MP_SET_UNSIGNED(mp_set_u64, 64)
#endif

#ifdef BN_MP_SET_I32_C
MP_SET_SIGNED(mp_set_i32, mp_set_u32, 32)
#endif

#ifdef BN_MP_SET_I64_C
MP_SET_SIGNED(mp_set_i64, mp_set_u64, 64)
#endif

#if defined(BN_MP_GET_I32_C) || defined(BN_MP_GET_U32_C)
MP_GET_SIGNED(mp_get_i32, mp_get_mag32, 32)
#endif

#if defined(BN_MP_GET_I64_C) || defined(BN_MP_GET_U64_C)
MP_GET_SIGNED(mp_get_i64, mp_get_mag64, 64)
#endif

#ifdef BN_MP_GET_MAG32_C
MP_GET_MAG(mp_get_mag32, 32)
#endif

#ifdef BN_MP_GET_MAG64_C
MP_GET_MAG(mp_get_mag64, 64)
#endif

#ifdef BN_MP_INIT_U32_C
MP_INIT_INT(mp_init_u32, mp_set_u32, uint32_t)
#endif

#ifdef BN_MP_INIT_I32_C
MP_INIT_INT(mp_init_i32, mp_set_i32, int32_t)
#endif

#ifdef BN_MP_INIT_U64_C
MP_INIT_INT(mp_init_u64, mp_set_u64, uint64_t)
#endif

#ifdef BN_MP_INIT_I64_C
MP_INIT_INT(mp_init_i64, mp_set_i64, int64_t)
#endif

#endif
