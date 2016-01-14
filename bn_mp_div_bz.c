#include <tommath.h>
#ifdef BN_MP_DIV_BZ_C

static int div2n1n(mp_int *a, mp_int *b, int n, mp_int *c, mp_int *d);
static int div3n2n(mp_int *a, mp_int *b, int n, mp_int *c, mp_int *d);

/*
   Fast division, Karatsuba-like. Assumes positive input!

   J. Burnikel and J. Ziegler, "Fast recursive division", Research Report
   MPI-I-98-1-022, Max-Planck-Institut fuer Informatik, Saarbruecken, Germany,
   October 1998.
   (available online)

   This implementation is slightly optimized. For easier understanding I
   recommend the paper mentioned above. Some explanations in the paper
   are slightly optimized. For easier understanding I recommend the code below.

   The numbered comments are taken verbatim from the description of the
   algorithm in the paper save some typographical adjusting for ASCII.


   Cutoffs:

   It equals out when the size of the numerator passes 750 limbs and is always
   faster at 775 limbs and more.

   The upper limit for the denominator is about 0.8 times the size of the
   numerator.

   To be on the safe side I recommend to set the minimum denominator to 50 and
   the minimum numerator to 250 and set the denominator free at a numerator size
   of 775. The upper limit 0.8 of the denominator holds for these sizes, too.

   TODO: write a little script to test the values in between.

   These values are too high, that is: this implementation is still too slow.
   The problem has the same reason as with ll recursive algorithms: allocating
   memory a lot of times. Here the amount of memory is known before (an upper
   limit to be exact) and this knowledge should be used. But using it involves
   heavy pointer juggling which would make this code nearly illegible.

*/
int mp_div_bz(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
{
   int i, j, k, m, n, sigma, t, err;
   mp_int aa, bb, cc, r, q;
   mp_digit msb_b, msb;

   if ((err = mp_init_multi(&aa, &bb, &cc, &r, &q, NULL)) != MP_OKAY) {
      return err;
   }
   // most significant bit of a limb
   msb = (mp_digit) 1 << ((mp_digit) MP_DIGIT_BIT - (mp_digit) 1);
   err = MP_OKAY;

   /*
    * Start value.
    * This can get changed in bncore.c, although the author cannot see any
    * reason to do so.
    */
   m = BURN_ZIEG_CHUNK_SIZE;
   k = 0;
   /*
    * 1. Set m = min {2^k | 2^k * DIV_LIMIT > s }
    *
    * From the paper (sec. 2.3)
    *
    * "This trick will minimize the number of 0s we waste when extending B and
    * leave us with smaller numbers in the base case of the recursion; our
    * tests have indicated that we gain a speedup of 25% in running time.So
    * we compute n = m * j with j minimal such that n is greater or equal
    * than s Then we extend B from the right to n digits. B has to be shifted
    * bitwise again for normalization until its most significant digit
    * fulfills b_(n-1) >= beta/2. Note that A has also to be extended and
    * shifted by the same amount."
    */
   while (m <= b->used) {
      m <<= 1;
      k++;
   }
   /*
    * 2. Set j = ceil(s/m) and n = j * m
    */
   j = (b->used + m - 1) / m;
   n = j * m;
   /*
    * 3. Set sigma = max {tau |2^tau * B < beta^n }
    */
   sigma = MP_DIGIT_BIT * (n - b->used);
   msb_b = b->dp[b->used - 1];
   while (msb_b < msb) {
      sigma++;
      msb_b <<= 1;
   }
   /*
    * 4a.  Set B = 2^sigma to normalize B
    */

   // We work on copy here, as always.
   if ((err = mp_mul_2d(a, sigma, &aa)) != MP_OKAY) {
      goto _ERR;
   }
   /*
    * 4b.  Set B = 2^sigma to shift A by the same amount as B
    */
   if ((err = mp_mul_2d(b, sigma, &bb)) != MP_OKAY) {
      goto _ERR;
   }
   /*
    * 5. Set t = min{ l >= 2 | A < beta^(l*n)/2 }
    *
    * In English: the most significant bit of the most significant digit of
    * the most significant chunk  must be 0 (zero) but 't' must be at
    * least 2 (two).
    */
   // ceil() to get a zero digit if aa.used is not divisible by n
   t = (aa.used + n - 1) / n;
   // is aa.used is divisible by n and the most significant bit of aa is not
   // equal to zero ad another chunk to guarantee condition of step 5.
   // A bit rough but simple
   if ((aa.used % n == 0) && ((aa.dp[aa.used - 1] & msb) != 0)) {
      t++;
   }
   // we need at least two chunks to make it worthwhile
   if (t < 2) {
      t = 2;
   }

   /*
    * 6. Split A conceptually into t blocks [A_(t-1)...A_0]beta^n = A.
    * Note that the leftmost bit of A_(t-1) is 0 by our choice of t.
    */

   // we know the start size of the variables which enables us to pre-allocate
   // them exactly (YMMV regarding the value of "exactly")

   // individual remainder and temporary variable (2*n saves us one realloc)
   if ((err = mp_grow(&r, 2 * n)) != MP_OKAY) {
      goto _ERR;
   }
   // individual quotient
   if ((err = mp_grow(&q, t * n)) != MP_OKAY) {
      goto _ERR;
   }
   // main quotient
   if ((err = mp_grow(&cc, t * n)) != MP_OKAY) {
      goto _ERR;
   }

   /*
    * 7. Set Z_(t-2) = [A_(t-1), A_(t-2) ].
    *
    * We do not do it at once but in parts. Only A_(t-1) gets put in Z
    * (we use the variable 'r' here), the second digit gets put in Z inside
    * the main loop to make things simpler
    */
   for (i = n * (t - 1), j = 0; i < aa.used; i++, j++) {
      r.dp[j] = aa.dp[i];
   }
   r.used = j;
   cc.used = 0;

   /*
    * For i from t-2 down to 0 do
    */
   t -= 2;
   while (t >= 0) {
      /*
       * (b) If i > 0, set Z_(i-1) = [R_i, A_(i-1)].
       */
      // TODO: avoid realloc'ing by allocating all at once.
      //       Disadvantage: needs an extended amount of pointer-juggling
      if ((err = mp_grow(&r, r.used + n)) != MP_OKAY) {
         goto _ERR;
      }
      // r.used, n and aa.used might be different, hence this pushing-around
      for (i = 0, j = n; i < r.used; i++, j++) {
         r.dp[j] = r.dp[i];
      }
      r.used = j;
      for (j = 0, i = t * n; j < n && i < aa.used; i++, j++) {
         r.dp[j] = aa.dp[i];
      }
      while (j < n) {
         r.dp[j] = 0;
      }
      /*
       * (a) Using algorithm D_(2n/1n) compute Q_i, R_i such that
       * Z_i = BQ_i + R_i.
       */
      if ((err = div2n1n(&r, &bb, n, &q, &r)) != MP_OKAY) {
         goto _ERR;
      }
      // add result to main quotient
      for (j = t * n, i = 0; i < q.used; i++, j++) {
         cc.dp[j] = q.dp[i];
      }
      for (; i < n; i++, j++) {
         cc.dp[j] = 0;
      }
      // The main quotient cannot get larger than (t-2) * n, so we can set
      // its size to (t-2)*n + q.used and leave it there. Zeros get chopped
      // off by the final clamp()
      if (cc.used == 0) {
         cc.used = j;
      }
      // position of next chunk
      t--;
   }
   /*
    * 9. Return Q = [Q_(t-2),...,Q_0] and R = R_0 * 2^(-sigma).
    */
   if ((err = mp_div_2d(&r, sigma, &r, NULL)) != MP_OKAY) {
      goto _ERR;
   }
   mp_clamp(&cc);
   // want quotient?
   if (c != NULL) {
      mp_exch(&cc, c);
   }
   // want remainder?
   // Burnikel & Ziegler proposed a variant of this algorithm that avoids
   // doing some parts of the computation of the remainder if no remainder is
   // wanted. This variant is not implemented here.
   if (d != NULL) {
      mp_exch(&r, d);
   }

_ERR:
   mp_clear_multi(&aa, &bb, &cc, &r, &q, NULL);
   return err;
}


static int div3n2n(mp_int *a, mp_int *b, int n, mp_int *c, mp_int *d)
{
   mp_int aa, bb, Qhat, Rhat, R1;
   int i, j, aorb, err;


   if ((err = mp_init_multi(&aa, &bb, &Qhat, &R1, &Rhat, NULL)) != MP_OKAY) {
      return err;
   }

   /*
    * 1. Split A into three parts A = [A_1, A_2, A_3 ] with A_i < beta^n.
    * 2. Split B into two parts B = [B_1, B_2] with B_i < beta^n.
    *
    * We need [A_1, A_2] and B_1 first, extract them
    */
   if ((err = mp_grow(&aa, 2 * n)) != MP_OKAY) {
      goto _ERR;
   }
   if ((err = mp_grow(&bb, n)) != MP_OKAY) {
      goto _ERR;
   }
   for (j = 0, i = n; i < a->used; i++, j++) {
      aa.dp[j] = a->dp[i];
   }
   aa.used = j;
   for (j = 0, i = n; i < b->used; i++, j++) {
      bb.dp[j] = b->dp[i];
   }
   bb.used = j;

   /*
    * 3. Distinguish the cases A_1 < B_1 or A1 >= B_1.
    *
    * Instead of extracting A_1 and B_1 and use the buildin mp_cmp, we do
    * the comparing by hand. It is a bit questionable which one is faster.
    */
   aorb = 0;
   if (aa.used > bb.used + n) {
      aorb = 0;
   } else if (bb.used + n > aa.used) {
      aorb = 1;
   } else {
      for (i = aa.used - 1, j = bb.used - 1; j >= 0; i--, j--) {
         if (aa.dp[i] < bb.dp[j]) {
            aorb = 1;
            break;
         } else if (bb.dp[j] < aa.dp[i]) {
            aorb = 0;
            break;
         }
      }
   }
   /*
    * (a) If A_1 < B_1 , compute Qhat = floor( [A_1,A_2 ]/B_1 ) with
    * remainder R1 using algorithm  D_(2n/1n).
    *
    * Just if you already wondered where the recursion part is.
    */
   if (aorb) {
      if ((err = div2n1n(&aa, &bb, n, &Qhat, &R1)) != MP_OKAY) {
         goto _ERR;
      }
   }
   /*
    *
    * (b) If A1 >= B1 , set Q = beta^n - 1 and set
    * R_1 = [A_1, A_2] - [B_1, 0] + [0, B_1]
    * ( = [A_1, A_2] - Qhat*B_1 ).
    *
    * We do [A_1, A_2] + B_1 - B_1 * 2^n here.
    */
   else {
      // set all of the bits of Qhat
      if ((err = mp_grow(&Qhat, n)) != MP_OKAY) {
         goto _ERR;
      }
      for (i = 0; i < n; ++i) {
         Qhat.dp[i] = MP_MASK;
      }
      Qhat.used = n;
      if ((err = mp_add(&aa, &bb, &R1)) != MP_OKAY) {
         goto _ERR;
      }
      if ((err = mp_lshd(&bb, n)) != MP_OKAY) {
         goto _ERR;
      }
      if ((err = mp_sub(&R1, &bb, &R1)) != MP_OKAY) {
         goto _ERR;
      }
   }

   /*
    * 4. Compute D = Qhat *  B_2 using Karatsuba multiplication.
    *
    * We don't have B_2 at hand, we have to extract it first
    */
   for (i = 0; i < n; i++) {
      bb.dp[i] = b->dp[i];
   }
   // With the hope that mp_mul() respects bb.used.
   // This wastes some memory, admitted.
   bb.used = n;

   if ((err = mp_mul(&Qhat, &bb, &aa)) != MP_OKAY) {
      goto _ERR;
   }
   /*
    * 5. Compute Rhat = R_1*beta^n + A_4 - D.
    *
    * That's wrong, must be A_3 instead of A_4.
    * Thank you very much, guys for _NOT_ updating your paper!
    */
   if ((err = mp_lshd(&R1, n)) != MP_OKAY) {
      goto _ERR;
   }
   // Get A_3 which is still hidden in the input
   // There is no mp_mask() in libtommathm we have to resort to mp_mod_2()
   // if we do not want to write one. It also saves some memory for the mask.
   if ((err = mp_mod_2d(a, MP_DIGIT_BIT * n, &Rhat)) != MP_OKAY) {
      goto _ERR;
   }
   if ((err = mp_add(&Rhat, &R1, &Rhat)) != MP_OKAY) {
      goto _ERR;
   }
   if ((err = mp_sub(&Rhat, &aa, &Rhat)) != MP_OKAY) {
      goto _ERR;
   }
   /*
    * 6. As long as Rhat < 0, repeat
    */
   while (Rhat.sign == MP_NEG) {
      /*
       * (a) Rhat = Rhat + B
       */
      if ((err = mp_add(&Rhat, b, &Rhat)) != MP_OKAY) {
         goto _ERR;
      }
      /*
       * (b) Qhat = Qhat - 1
       */
      if ((err = mp_sub_d(&Qhat, 1, &Qhat)) != MP_OKAY) {
         goto _ERR;
      }
   }
   /*
    * 7. Return R = Rhat, Q = Qhat.
    */
   mp_exch(&Qhat, c);
   mp_exch(&Rhat, d);
_ERR:
   mp_clear_multi(&aa, &bb, &Rhat, &R1, &Qhat, NULL);
   return err;
}


static int div2n1n(mp_int *a, mp_int *b, int n, mp_int *c, mp_int *d)
{
   mp_int q1, q2, r;
   int i, j, half, err;

   err = MP_OKAY;
   /*
    * 1. If n is odd or smaller than some convenient constant,
    * compute Q and R by school division and return.
    *
    * A "convenient constant". Don't you love it, too, when the authors are
    * so incredibly exact?
    *
    * But a minimum is available: the whole construct and reason for being
    * asymptotically faster than school-division is one multiplication in
    * the algorithm D_(3n/2n) (above) with both participants larger than
    * the Karatsuba cutoff. This product is Qhat * B_2. The size of B_2
    * is half the size of the denominator, the size of Qhat depends on the
    * outcome of floor(A_1 / B_1), so #Qhat <= #B_2.
    * That means that the "convenient constant" must be of size
    * 2 * karatsuba-cutoff at least.
    * The overall overhead is relatively small, I found it sufficient to add
    * another 10 limbs as angst-allowance, but as always: YMMV.
    *
    * You can use the odd n, too if you shift a and b left one bit and add one
    * to n and at the end the remainder right one bit. But because of the
    * recursion one deep copy is necessary which seems to ruin the gain.
    */
   if (n % 1 || n <= DIV_BURN_ZIEG_CUTOFF) {
      return mp_div_school(a, b, c, d);
   }

   if ((err = mp_init_multi(&q1, &q2, &r, NULL)) != MP_OKAY) {
      return err;
   }

   // the length of one large digit e.g. #B_1
   half = n >> 1;

   /*
    * 2. Split A into four parts A = [A_1,...,A_4 ] with A_i < beta^(n/2).
    * Split B into two parts  B = [B_1, B_2] with B_i < beta^(n/2).
    *
    * We need the parts of A in chunks of 3 and 1 for the individual runs
    * of D_(3n/2n). The partition of B gets done by D_(3n/2n), no reason to
    * bother with it here.
    *
    * We use 'r' for both, the remainder and as a temporary variable
    */
   if (a->used >= half) {
      if ((err = mp_grow(&r, a->used - half)) != MP_OKAY) {
         goto _ERR;
      }
   }
   // r = [A_1,A_2,A_3]
   for (i = 0, j = half; j < a->used; ++i, ++j) {
      r.dp[i] = a->dp[j];
   }
   r.used = i;

   /*
    * 3. Compute the high part Q_1 of floor(A/B) as
    * Q_1 = floor( [A_1, A_2, A_3] / [B_1, B_2] ) with remainder
    * R_1 = [R_(1,1), R_(1,2)] using algorithm D_(3n/2n).
    *
    */
   if ((err = div3n2n(&r, b, half, &q1, &r)) != MP_OKAY) {
      goto _ERR;
   }

   /*
    * 4. Compute the low part Q_2 of floor(A/B) as
    * Q_2 = floor( [R_(1,1), R_(1,2), A_4] / [B_1, B_2] ) with remainder
    * R using algorithm D_(3n/2n).
    *
    */
   if ((err = mp_grow(&r, r.used + half)) != MP_OKAY) {
      goto _ERR;
   }
   // make room for A_4 (lshd(half))
   for (i = r.used - 1; i >= 0; --i) {
      r.dp[i + half] = r.dp[i];
   }
   // A_4
   for (i = 0; i < half && i < a->used; ++i) {
      r.dp[i] = a->dp[i];
   }
   while (i < half) {
      r.dp[i++] = 0;
   }
   r.used += half;
   if ((err = div3n2n(&r, b, half, &q2, &r)) != MP_OKAY) {
      goto _ERR;
   }
   /*
    * 5. Return Q = [Q_1, Q_2] and R.
    */
   if ((err = mp_grow(&q2, q1.used + half)) != MP_OKAY) {
      goto _ERR;
   }
   // q2 might be smaller than half, pad with zeros
   for (i = q2.used; i < half; i++) {
      q2.dp[i] = 0;
   }
   for (j = 0; j < q1.used; j++, i++) {
      q2.dp[i] = q1.dp[j];
   }
   q2.used = i;

   mp_exch(&q2, c);
   mp_exch(&r, d);

_ERR:
   mp_clear_multi(&r, &q2, &q1, NULL);
   return err;
}
#endif

