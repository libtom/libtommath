#include "tommath_private.h"
#ifdef MP_PACK_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* based on gmp's mpz_export.
 * see http://gmplib.org/manual/Integer-Import-and-Export.html
 */
mp_err mp_pack(void *rop, size_t maxcount, size_t *written, mp_order order, size_t size,
               mp_endian endian, size_t nails, const mp_int *op)
{
   mp_err err = MP_OKAY;
   size_t odd_nails, nail_bytes, i, j, count;
   uint8_t odd_nail_mask;

   mp_int t;

   count = mp_pack_count(op, nails, size);

   if (count > maxcount) {
      err = MP_BUF;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   if ((err = mp_init_copy(&t, op)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR);

   if (endian == MP_NATIVE_ENDIAN) {
      MP_GET_ENDIANNESS(endian);
   }

   odd_nails = (nails % 8u);
   odd_nail_mask = 0xff;
   for (i = 0u; i < odd_nails; ++i) {
      odd_nail_mask ^= (uint8_t)(1u << (7u - i));
   }
   nail_bytes = nails / 8u;

   for (i = 0u; i < count; ++i) {
      for (j = 0u; j < size; ++j) {
         uint8_t *byte = (uint8_t *)rop +
                         (((order == MP_LSB_FIRST) ? i : ((count - 1u) - i)) * size) +
                         ((endian == MP_LITTLE_ENDIAN) ? j : ((size - 1u) - j));

         if (j >= (size - nail_bytes)) {
            *byte = 0;
            continue;
         }

         *byte = (uint8_t)((j == ((size - nail_bytes) - 1u)) ? (t.dp[0] & odd_nail_mask) : (t.dp[0] & 0xFFuL));

         if ((err = mp_div_2d(&t, (j == ((size - nail_bytes) - 1u)) ? (int)(8u - odd_nails) : 8, &t, NULL)) != MP_OKAY)
            MP_TRACE_ERROR(err, LTM_ERR_1);
      }
   }

   if (written != NULL) {
      *written = count;
   }

LTM_ERR_1:
   mp_clear(&t);
LTM_ERR:
   return err;
}

#endif
