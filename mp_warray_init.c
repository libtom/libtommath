#include "tommath_private.h"
#ifdef MP_WARRAY_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

static mp_err s_warray_init(size_t n_alloc, bool preallocate)
{
   size_t n;
   if (s_mp_warray.l_free != NULL || s_mp_warray.l_used != NULL) {
      return MP_VAL;
   }

   s_mp_warray.l_free = MP_CALLOC(n_alloc, sizeof(*(s_mp_warray.l_free)));
   s_mp_warray.l_used = MP_CALLOC(n_alloc, sizeof(*(s_mp_warray.l_used)));
   if (s_mp_warray.l_free == NULL || s_mp_warray.l_used == NULL) {
      s_mp_warray_free(n_alloc);
      return MP_MEM;
   }

   if (preallocate) {
      for (n = 0; n < n_alloc; ++n) {
         s_mp_warray.l_free[n].warray = MP_CALLOC(MP_WARRAY, sizeof(mp_word));
         if (s_mp_warray.l_free[n].warray == NULL) {
            while (n > 0) {
               n--;
               MP_FREE(s_mp_warray.l_free[n].warray, MP_WARRAY * sizeof(mp_word));
               s_mp_warray.l_free[n].warray = NULL;
            }
            s_mp_warray_free(n_alloc);
            return MP_MEM;
         }
      }
      s_mp_warray.allocated = n_alloc;
   }

   s_mp_warray.usable = n_alloc;
   return MP_OKAY;
}

mp_err mp_warray_init(size_t n_alloc, bool preallocate)
{
   if (MP_HAS(MP_SMALL_STACK_SIZE)) return s_warray_init(n_alloc, preallocate);
   return MP_ERR;
}

#endif
