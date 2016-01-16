#include <tommath.h>
#ifdef BN_MP_BALANCE_MUL_C


/* single-digit multiplication with b as the single-digit */
int mp_balance_mul(mp_int *a, mp_int *b, mp_int *c)
{
   int e, count, len_a, len_b, nblocks, i, j, bsize;
   mp_int a0, tmp, A , B;

   len_a = a->used;
   len_b = b->used;

   nblocks = MAX(a->used, b->used) / MIN(a->used, b->used);
   bsize = MIN(a->used, b->used) ;

   if ((e = mp_init_size(&a0, bsize + 2)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_init_multi(&tmp, &A, &B, NULL)) != MP_OKAY) {
      mp_clear(&a0);
      return e;
   }

   /* Make sure that A is the larger one*/
   if (len_a < len_b) {
      mp_copy(a,&B);
      mp_copy(b,&A);
      //B = *a;
      //A = *b;
   } else {
      mp_copy(a,&A);
      mp_copy(b,&B);
      // B = *b;
      // A = *a;
   }
   for (i = 0, j=0; i < nblocks; i++) {
      /* Cut a slice off of a */
      a0.used = 0;
      for (count = 0; count < bsize; count++) {
         a0.dp[count] = A.dp[ j++ ];
         a0.used++;
      }
      /* Multiply with b */
      if ((e = mp_mul(&a0, &B, &tmp)) != MP_OKAY) {
         goto ERR;
      }
      /* Shift tmp to the correct position */
      if ((e = mp_lshd(&tmp, bsize * i)) != MP_OKAY) {
         goto ERR;
      }
      /* Add to output. No carry needed */
      if ((e = mp_add(c, &tmp, c)) != MP_OKAY) {
         goto ERR;
      }
   }
   /* The left-overs; there are always left-overs */
   if (j < A.used) {
      a0.used = 0;
      for (count = 0; j < A.used; count++) {
         a0.dp[count] = A.dp[ j++ ];
         a0.used++;
      }
      if ((e = mp_mul(&a0, &B, &tmp)) != MP_OKAY) {
         goto ERR;
      }
      if ((e = mp_lshd(&tmp, bsize * i)) != MP_OKAY) {
         goto ERR;
      }
      if ((e = mp_add(c, &tmp, c)) != MP_OKAY) {
         goto ERR;
      }
   }

ERR:
   mp_clear_multi(&a0, &tmp, &A, &B, NULL);
   return MP_OKAY;
}


int mp_balance_recursive(mp_int *a, mp_int *b, mp_int *c)
{
   int e, len_a, len_b, count;
   mp_int a_0, a_1, A , B;

   len_a = a->used;
   len_b = b->used;

   if (len_a < len_b) {
      B = *a;
      A = *b;
   } else {
      B = *b;
      A = *a;
   }
   /*
    * Cut larger one in two parts a1, a0 with the smaller part a0 of the same
    * length as the smaller input number b_0. Work on copy to make things simpler
    */
   if ((e = mp_init_size(&a_0, B.used + 1)) != MP_OKAY) {
      return e;
   }
   a_0.used = B.used;
   if ((e = mp_init_size(&a_1, A.used - B.used + 1)) != MP_OKAY) {
      mp_clear(&a_0);
      return e;
   }
   a_1.used = A.used - B.used;
   /* fill smaller part a_0 */
   for (count = 0; count < B.used; count++) {
      a_0.dp[count] = A.dp[count];
   }
   /* fill bigger part a_1 with the counter already at the right place */
   for (; count < A.used; count++) {
      a_1.dp[count - B.used] = A.dp[count];
   }
   /* a_1 = a_1 * b_0 */
   /* a_1 and b_0 are of different size and might allow for another round */
   if ((e = mp_mul(&a_1, &B, &a_1)) != MP_OKAY) {
      goto ERR;
   }
   /* a_1 = a_1 * 2^(length(b_0)) */
   if ((e = mp_lshd(&a_1, B.used)) != MP_OKAY) {
      goto ERR;
   }
   /* a_0 = a_0 * b_0 */
   if ((e = mp_mul(&a_0, &B, &a_0)) != MP_OKAY) {
      goto ERR;
   }
   /* c = a_1 + a_0 */
   if ((e = mp_add(&a_1, &a_0, c)) != MP_OKAY) {
      goto ERR;
   }
ERR:
   mp_clear_multi(&a_0, &a_1, NULL);
   return e;
}

#endif

