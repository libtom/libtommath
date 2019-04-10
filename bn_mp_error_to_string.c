#include "tommath_private.h"
#ifdef BN_ERROR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

static const struct {
   int code;
   const char *msg;
} msgs[] = {
   { MP_OKAY, "Successful" },
   { MP_MEM,  "Out of heap" },
   { MP_VAL,  "Value out of range" }
};

/* return a char * string for a given code */
const char *mp_error_to_string(int code)
{
   size_t x;

   /* scan the lookup table for the given message */
   for (x = 0; x < (sizeof(msgs) / sizeof(msgs[0])); x++) {
      if (msgs[x].code == code) {
         return msgs[x].msg;
      }
   }

   /* generic reply for invalid code */
   return "Invalid error code";
}

#endif
