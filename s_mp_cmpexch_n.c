#include "tommath_private.h"
#ifdef S_MP_CMPEXCH_N_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifdef __GNUC__
#define S_CMPEXCH_N_GCC_C
static bool s_cmpexch_n_gcc(void **ptr, void **expected, void *desired)
{
   return __atomic_compare_exchange_n(ptr, expected, desired, true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
}
#endif

#ifdef _MSC_VER
#define S_CMPEXCH_N_MSVC_C

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#ifndef WINVER
#define WINVER 0x0501
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static bool s_cmpexch_n_msvc(void * volatile *ptr, void **expected, void *desired)
{
   return InterlockedCompareExchangePointer(ptr, desired, *(expected));
}
#endif

bool s_cmpexch_n_gcc(void **ptr, void **expected, void *desired);
bool s_cmpexch_n_msvc(void * volatile *ptr, void **expected, void *desired);

bool s_mp_cmpexch_n(void **ptr, void **expected, void *desired)
{
   if (MP_HAS(S_CMPEXCH_N_GCC)) return s_cmpexch_n_gcc(ptr, expected, desired);
   if (MP_HAS(S_CMPEXCH_N_MSVC)) return s_cmpexch_n_msvc(ptr, expected, desired);
   return false;
}

#endif
