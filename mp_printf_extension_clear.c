#include "tommath_private.h"
#ifdef MP_PRINTF_EXTENSION_CLEAR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



#if (!(defined(_MSC_VER)) &&  !(defined(MP_NO_FILE)) && (defined(GLIBC_VERSION)) && (GLIBC_VERSION > 2009) )
#include <printf.h>
/* Unregister printf extensions */
void mp_printf_extension_clear(void)
{
   /* The modifiers cannot be "unregistered" only the specifiers */
   register_printf_specifier('d', NULL, NULL);
   register_printf_specifier('x', NULL, NULL);
   register_printf_specifier('o', NULL, NULL);
   register_printf_specifier('b', NULL, NULL);
   register_printf_specifier('@', NULL, NULL);
}
#else
void mp_printf_extension_clear(void)
{
}
#endif


#endif
