#include "tommath_private.h"
#ifdef S_MP_TO_RADIX_POWER_OF_TWO_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Stores a bignum as an ASCII string in a given radix that must be a power of two.
 *
 * Stores upto "size - 1" chars and always a NULL byte, puts the number of characters
 * written, including the '\0', in "written".
 */

/* TODO: A macro instead? */
static int s_mp_get_bit_group(mp_word w, int position, int size)
{
   mp_word one = 1;
   return (int)(((one << size) - (mp_word)(1)) & (w >> (position - size)));
}

mp_err s_mp_to_radix_power_of_two(const mp_int *a, char *str, size_t maxlen, size_t *written, int radix_bit)
{
   /* Counter of base b digits */
   size_t  digs = 0u;

   /* floor(log_2(a)) */
   int len;
   /* Bit-size of most significant base b digit */
   int first;

   /* Counter of bits residing in buffer */
   int w_bit = 0;
   /* Index into a->dp[] (or use a pointer instead?) */
   int aidx;
   /* Holds the digit to be converted. TODO: digit does not exceed the size of 6 bits. */
   int c;
   /*
       Buffer for the bits.
       We can use the individual limbs directly but it is simpler that way and
       as we have that type already, why not use it.
       Caveats: it might not be a native integer and operations on it are slower
       in that case but pushing some bits in and reading some bits out are not
       that expensive.
    */
   mp_word W;

   /* TODO: no buffer needed for radix = 2, just convert bit by bit. Worth the extra code?
            We could outsource it and use an MP_HAS() construct?
   */

   /* LTM big integers are little endian and we start with the most significant limb first */
   aidx = a->used - 1;

   len = mp_count_bits(a);

   /* Load a->dp[a->used -1] and a->dp[a->used -2] if available into W to fill the buffer */
   W = a->dp[aidx--];
   if (a->used > 1) {
      W <<= MP_DIGIT_BIT;
      /* There might be rubbish in the MSBs of the limbs, mask them off */
      W += a->dp[aidx--] & MP_MASK;
      w_bit =(len - ((a->used - 1) * MP_DIGIT_BIT)) + MP_DIGIT_BIT;
   } else {
      w_bit =len;
   }

   /* The first digit in base b might not be "complete", we need to convert it first */
   first = len % radix_bit;

   /* TODO: Squeeze it in the main loop. But it needs a couple of exceptions there
            if the input is very small. Branching is expensive. Still worth it? */

   /* Get most significant digit in base b if it has less bits than radix_bit. */
   if (first != 0) {
      c = s_mp_get_bit_group(W, w_bit, first);
      *str++ = s_mp_radix_map[c];
      digs++;
      if (--maxlen < 1u) {
         return MP_BUF;
      }
      /* We have one digit pulled out of the buffer, adjust bit-count */
      w_bit -= first;
   }
   /*
      That above was the digit of base b that is the remainder of len/radix_bit.
      Hence the rest len-(len % radix_bit) is divisible by log_2(b).
   */
   for (;;) {
      /* Work that buffer: pull bits, convert and fill output. */
      while (w_bit >= radix_bit) {
         c = s_mp_get_bit_group(W, w_bit, radix_bit);
         *str++ = s_mp_radix_map[c];
         digs++;
         if (--maxlen < 1u) {
            return MP_BUF;
         }
         w_bit -= radix_bit;
      }
      /*
         Fill buffer with another limb if available.
         We can only add one limb at a time because radix_size + 2 * MP_DIGIT_BIT
         can be too big for some limb-sizes.
       */
      if (aidx >= 0) {
         W <<= MP_DIGIT_BIT;
         W += (a->dp[aidx--]) & MP_MASK;
         w_bit += MP_DIGIT_BIT;
      } else {
         break;
      }
   }

   /* append a NULL so the string is properly terminated */
   *str = '\0';
   digs++;
   if (written != NULL) {
      *written = mp_isneg(a) ? (digs + 1u): digs;
   }

   return MP_OKAY;
}

#endif
