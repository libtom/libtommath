#include "tommath_private.h"
#ifdef MP_READ_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* TODO: strlen() shouldn't be problematic but you'll never know */
#ifdef MP_USE_MEMOPS
#  include <string.h>
#  define MP_STRLEN(s) strlen(s)
#else
static size_t s_mp_strlen(const char *s)
{
   const char *p;
   p = s;
   while (*p != '\0') {
      p++;
   }
   return (size_t)(p - s);
}
#  define MP_STRLEN(s) s_mp_strlen(s)
#endif

/* read a string [ASCII] in a given radix */
mp_err mp_read_radix(mp_int *a, const char *str, int radix)
{

   mp_err   err = MP_OKAY;
   mp_sign  sign = MP_ZPOS;
   size_t slen, slen_2;

   /* make sure the radix is ok */
   if ((radix < 2) || (radix > 64)) {
      return MP_VAL;
   }

   /* if the leading digit is a
    * minus set the sign to negative.
    */
   if (*str == '-') {
      ++str;
      sign = MP_NEG;
   }

   slen = MP_STRLEN(str);
   /* "slen" is log_b(str) with b = radix and with log_a(x) = log_b(x)/log_b(a) we can use log_2(str) = slen/log_b(2)
      but we do not have floats (there is a fixed point version of log_2(x) in s_mp_fp_log_d.c, though).
      We could use a table with rational approximations instead which costs quite some stack-memory because
      we would need it for every MP_DIGIT_BIT size..

      So to keep things simple we restrict our length checks to bases 10 and powers of two for now.

      Bases that are a power of two are the easiest: just multiply "slen" with log_2(radix) to get the bits
      necessary to compute the bit-size. The result shall not be  bigger than (MP_MAX_DIGIT_COUNT - 2) * MP_DIGIT_BIT.
      This is in almost all cases an overestimate  because the MSD is most likely not full but only by a couple of bits,
      at most 63 in case of radix = 64.  That means that at most 5 limbs (MP_16BIT), 3 limbs (MP_28BIT and MP_31BIT),
      or 2 limbs (MP_64BIT) are going to waste.

      For base 10 and (INT_MAX = 2^(31) - 1) the limits are:
      MP_DIGIT_BIT   limit     tested          notes
      15              2183       y        (with INT_MAX = 32767) which is MP_MAX_DIGIT_COUNT - 1
      28            76695844     y
      31            69273664     n        Not tested because there is no FFT for MP_31BIT and I
                                          don't have the patients of a Buddha.
      60            35791392     y

   */

   if (MP_IS_2EXPT((unsigned int)radix) &&
       ((slen * (size_t) s_mp_log2_radix[radix]) > ((MP_MAX_DIGIT_COUNT - 2) * MP_DIGIT_BIT))) {
      return MP_OVF;
   } else  if ((radix == 10) && (slen >
#if (MP_DIGIT_BIT == 15)
                                 2183
#elif (MP_DIGIT_BIT == 28)
                                 76695844
#elif (MP_DIGIT_BIT == 31)
                                 69273664
#elif (MP_DIGIT_BIT == 60)
                                 35791392
#endif
                                )) {
      return MP_OVF;
   }

   /* Roughly (over)estimate bit-size for cutoff by assuming slen to be ceil(log_{radix}(input))
      so  bits(slen) ~ slen_{radix} * ceil(log_2(radix)) */
   slen_2 = slen * (size_t)(s_mp_log2_radix[radix] + 1);

   mp_zero(a);
   /* Try faster version first */
   if (MP_HAS(S_MP_FASTER_READ_RADIX) && (slen_2 > (size_t)MP_RADIX_READ_CUTOFF)) {
      if ((err = s_mp_faster_read_radix(a, str, 0, slen, radix)) != MP_OKAY)                             goto LTM_ERR;
   } else if (MP_HAS(S_MP_SLOWER_READ_RADIX)) {
      if ((err = s_mp_slower_read_radix(a, str, 0, slen, radix)) != MP_OKAY)                             goto LTM_ERR;
   }

   /* set the sign only if a != 0 */
   if (!mp_iszero(a)) {
      a->sign = sign;
   }

LTM_ERR:
   return err;
}

#endif
