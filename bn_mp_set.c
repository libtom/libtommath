#include "tommath_private.h"

/* Fancy macro to set an MPI from another type.
 * There are several things assumed:
 *  x is the counter
 *  a is the pointer to the MPI
 *  b is the original value that should be set in the MPI.
 */
#define MP_SET_UNSIGNED(w)                                              \
    void mp_set_u##w(mp_int * a, uint##w##_t b)                         \
    {                                                                   \
        int i = 0;                                                      \
        while (b != 0u) {                                               \
            a->dp[i++] = ((mp_digit)b & MP_MASK);                       \
            if (w <= MP_DIGIT_BIT) { break; }                           \
            b >>= ((w <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);             \
        }                                                               \
        a->used = i;                                                    \
        a->sign = MP_ZPOS;                                              \
        MP_ZERO_DIGITS(a->dp, a->alloc - a->used);                      \
    }
#define MP_SET_SIGNED(w)                                                \
    void mp_set_i##w(mp_int * a, int##w##_t b)                          \
    {                                                                   \
        mp_set_##uname(a, b < 0 ? -(uint##w##_t)b : (uint##w##_t)b);    \
        if (b < 0) { a->sign = MP_NEG; }                                \
    }
#define MP_INIT_INT(name, type)                          \
    mp_err mp_init_##name(mp_int * a, type b)            \
    {                                                    \
        mp_err err;                                      \
        if ((err = mp_init(a)) != MP_OKAY) {             \
            return err;                                  \
        }                                                \
        mp_set_##name(a, b);                             \
        return MP_OKAY;                                  \
    }
#define MP_GET_MAG(w)                                                   \
    uint##w##_t mp_get_mag##w(const mp_int* a)                          \
    {                                                                   \
        unsigned i = MP_MIN((unsigned)a->used, (unsigned)((w + MP_DIGIT_BIT - 1) / MP_DIGIT_BIT)); \
        uint##w##_t res = 0;                                            \
        while (i --> 0) {                                               \
            res <<= ((w <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);           \
            res |= (uint##w##_t)a->dp[i];                               \
            if (w <= MP_DIGIT_BIT) { break; }                           \
        }                                                               \
        return res;                                                     \
    }
#define MP_GET_SIGNED(w)                                                \
    int##w##_t mp_get_i##w(const mp_int* a)                             \
    {                                                                   \
        uint64_t res = mp_get_mag##w(a);                                \
        return a->sign == MP_NEG ? (int##w##_t)-res : (int##w##_t)res;  \
    }

#ifdef BN_MP_SET_U32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_SET_UNSIGNED(32)
#endif

#ifdef BN_MP_SET_U64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_SET_UNSIGNED(64)
#endif

#ifdef BN_MP_SET_I32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_SET_SIGNED(32)
#endif

#ifdef BN_MP_SET_I64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_SET_SIGNED(64)
#endif

#ifdef BN_MP_GET_I32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_GET_SIGNED(32)
#endif

#ifdef BN_MP_GET_I64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_GET_SIGNED(64)
#endif

#ifdef BN_MP_GET_MAG32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_GET_MAG(32)
#endif

#ifdef BN_MP_GET_MAG64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_GET_MAG(64)
#endif

#ifdef BN_MP_INIT_U32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_INIT_INT(u32, uint32_t)
#endif

#ifdef BN_MP_INIT_U64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_INIT_INT(u64, uint64_t)
#endif

#ifdef BN_MP_INIT_I32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_INIT_INT(i32, int32_t)
#endif

#ifdef BN_MP_INIT_I64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
MP_INIT_INT(i64, int64_t)
#endif
