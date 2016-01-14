#include <tommath.h>
#ifdef BN_MP_SUBTRACT_FACTORED_FACTORIALS_C
int mp_subtract_factored_factorials(long *subtrahend,
                                    unsigned long l_subtrahend,
                                    long *minuend,
                                    unsigned long l_minuend,
                                    long **difference,
                                    unsigned long /* Vive */ *l_difference /*!*/
                                   )
{
   unsigned long k, counter=0;
   unsigned long max_length = MAX(l_subtrahend,l_minuend);
   unsigned long min_length = MIN(l_subtrahend,l_minuend);
   long d,p,d_1,d_2;
   /* TODO: check for sizes > 0 here */

   /* Things work a bit different from ordinary arithmetic from now on */
   *difference = malloc(sizeof(unsigned long)*(max_length+1));
   if (*difference == NULL) {
      return MP_MEM;
   }

   /* Loop over smaller chunk first */
   for (k=0; k<min_length; k+=2) {
      /* both have same length, we can take the value of the prime from any */
      p = subtrahend[k];
      /*d = subtrahend[k+1] - minuend[k+1];*/
      d_1 = subtrahend[k+1];
      d_2 = minuend[k+1];
      /* handle over/underflow */
      if ((d_2 > 0) && (d_1 < LONG_MIN + d_2)) {
         return MP_VAL;
      }
      if ((d_2 < 0) && (d_1 > LONG_MAX + d_2)) {
         return MP_VAL;
      }
      d = d_1 - d_2;
      (*difference)[counter]   = p;
      (*difference)[counter+1] = d;
      counter += 2;
   }
   /* We need to find out which one is the smaller array and we have basically
      two ways to approach the problem:
        a) complicated and error-prone pointer juggling
        b) just two loops
      Here I have chosen b.
   */

   /* If both arrays have the same length we can just stop here */
   if (max_length == min_length) {
      /* We mad nothing dirty, so there's nothing to clean up here, let's just
         grab our stuff and run */
      *l_difference = counter;
      return MP_OKAY;
   }

   /* If the subtrahend is bigger we subtract zeros, so simply copy */
   if (l_subtrahend >= l_minuend) {
      for (k=min_length; k<max_length; k+=2) {
         p = subtrahend[k];
         d = subtrahend[k+1];
         (*difference)[counter]   = p;
         (*difference)[counter+1] = d;
         counter += 2;
      }
   }
   /* If the minuend is bigger we subtract from zeros, so negate before copy*/
   else {
      for (k=min_length; k<max_length; k+=2) {
         p = minuend[k];
         /* Yes, even negation can overflow */
         d_1 = minuend[k+1];
         if (d_1 == LONG_MIN) {
            return MP_VAL;
         }
         d = -d_1;
         (*difference)[counter]   = p;
         (*difference)[counter+1] = d;
         counter += 2;
      }
   }
   *l_difference = counter;
   return MP_OKAY;
}

#endif
