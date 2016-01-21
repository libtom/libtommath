#include <tommath.h>
#ifdef BN_MP_TOOM_MUL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */

/* multiplication using the Toom-Cook 3-way algorithm
 *
 * Much more complicated than Karatsuba but has a lower
 * asymptotic running time of O(N**1.464).  This algorithm is
 * only particularly useful on VERY large inputs
 * (we're talking 1000s of digits here...).
*/
int mp_toom_mul(mp_int *a, mp_int *b, mp_int *c)
{
   mp_int w0, w1, w2, w3, w4, tmp1, tmp2, a0, a1, a2, b0, b1, b2;
   int res, B;

   /* init temps */
   if ((res = mp_init_multi(&w0, &w1, &w2, &w3, &w4,
                            &a0, &a1, &a2, &b0, &b1,
                            &b2, &tmp1, &tmp2, NULL)) != MP_OKAY) {
      return res;
   }

   /* B */
   B = MIN(a->used, b->used) / 3;

   /* a = a2 * B**2 + a1 * B + a0 */
   if ((res = mp_mod_2d(a, DIGIT_BIT * B, &a0)) != MP_OKAY) {
      goto ERR;
   }

   if ((res = mp_copy(a, &a1)) != MP_OKAY) {
      goto ERR;
   }
   mp_rshd(&a1, B);
   mp_mod_2d(&a1, DIGIT_BIT * B, &a1);

   if ((res = mp_copy(a, &a2)) != MP_OKAY) {
      goto ERR;
   }
   mp_rshd(&a2, B*2);

   /* b = b2 * B**2 + b1 * B + b0 */
   if ((res = mp_mod_2d(b, DIGIT_BIT * B, &b0)) != MP_OKAY) {
      goto ERR;
   }

   if ((res = mp_copy(b, &b1)) != MP_OKAY) {
      goto ERR;
   }
   mp_rshd(&b1, B);
   mp_mod_2d(&b1, DIGIT_BIT * B, &b1);

   if ((res = mp_copy(b, &b2)) != MP_OKAY) {
      goto ERR;
   }
   mp_rshd(&b2, B*2);

   /* w0 = a0*b0 */
   if ((res = mp_mul(&a0, &b0, &w0)) != MP_OKAY) {
      goto ERR;
   }

   /* w4 = a2 * b2 */
   if ((res = mp_mul(&a2, &b2, &w4)) != MP_OKAY) {
      goto ERR;
   }

   /* w1 = (a2 + 2(a1 + 2a0))(b2 + 2(b1 + 2b0)) */
   if ((res = mp_mul_2(&a0, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp1, &a1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_mul_2(&tmp1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp1, &a2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((res = mp_mul_2(&b0, &tmp2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp2, &b1, &tmp2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_mul_2(&tmp2, &tmp2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp2, &b2, &tmp2)) != MP_OKAY) {
      goto ERR;
   }

   if ((res = mp_mul(&tmp1, &tmp2, &w1)) != MP_OKAY) {
      goto ERR;
   }

   /* w3 = (a0 + 2(a1 + 2a2))(b0 + 2(b1 + 2b2)) */
   if ((res = mp_mul_2(&a2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp1, &a1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_mul_2(&tmp1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp1, &a0, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((res = mp_mul_2(&b2, &tmp2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp2, &b1, &tmp2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_mul_2(&tmp2, &tmp2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp2, &b0, &tmp2)) != MP_OKAY) {
      goto ERR;
   }

   if ((res = mp_mul(&tmp1, &tmp2, &w3)) != MP_OKAY) {
      goto ERR;
   }


   /* w2 = (a2 + a1 + a0)(b2 + b1 + b0) */
   if ((res = mp_add(&a2, &a1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp1, &a0, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&b2, &b1, &tmp2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp2, &b0, &tmp2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_mul(&tmp1, &tmp2, &w2)) != MP_OKAY) {
      goto ERR;
   }

   /* now solve the matrix

      0  0  0  0  1
      1  2  4  8  16
      1  1  1  1  1
      16 8  4  2  1
      1  0  0  0  0

      using 12 subtractions, 4 shifts,
             2 small divisions and 1 small multiplication
    */

   /* r1 - r4 */
   if ((res = mp_sub(&w1, &w4, &w1)) != MP_OKAY) {
      goto ERR;
   }
   /* r3 - r0 */
   if ((res = mp_sub(&w3, &w0, &w3)) != MP_OKAY) {
      goto ERR;
   }
   /* r1/2 */
   if ((res = mp_div_2(&w1, &w1)) != MP_OKAY) {
      goto ERR;
   }
   /* r3/2 */
   if ((res = mp_div_2(&w3, &w3)) != MP_OKAY) {
      goto ERR;
   }
   /* r2 - r0 - r4 */
   if ((res = mp_sub(&w2, &w0, &w2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_sub(&w2, &w4, &w2)) != MP_OKAY) {
      goto ERR;
   }
   /* r1 - r2 */
   if ((res = mp_sub(&w1, &w2, &w1)) != MP_OKAY) {
      goto ERR;
   }
   /* r3 - r2 */
   if ((res = mp_sub(&w3, &w2, &w3)) != MP_OKAY) {
      goto ERR;
   }
   /* r1 - 8r0 */
   if ((res = mp_mul_2d(&w0, 3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_sub(&w1, &tmp1, &w1)) != MP_OKAY) {
      goto ERR;
   }
   /* r3 - 8r4 */
   if ((res = mp_mul_2d(&w4, 3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_sub(&w3, &tmp1, &w3)) != MP_OKAY) {
      goto ERR;
   }
   /* 3r2 - r1 - r3 */
   if ((res = mp_mul_d(&w2, 3, &w2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_sub(&w2, &w1, &w2)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_sub(&w2, &w3, &w2)) != MP_OKAY) {
      goto ERR;
   }
   /* r1 - r2 */
   if ((res = mp_sub(&w1, &w2, &w1)) != MP_OKAY) {
      goto ERR;
   }
   /* r3 - r2 */
   if ((res = mp_sub(&w3, &w2, &w3)) != MP_OKAY) {
      goto ERR;
   }
   /* r1/3 */
   if ((res = mp_div_3(&w1, &w1, NULL)) != MP_OKAY) {
      goto ERR;
   }
   /* r3/3 */
   if ((res = mp_div_3(&w3, &w3, NULL)) != MP_OKAY) {
      goto ERR;
   }

   /* at this point shift W[n] by B*n */
   if ((res = mp_lshd(&w1, 1*B)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_lshd(&w2, 2*B)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_lshd(&w3, 3*B)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_lshd(&w4, 4*B)) != MP_OKAY) {
      goto ERR;
   }

   if ((res = mp_add(&w0, &w1, c)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&w2, &w3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&w4, &tmp1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((res = mp_add(&tmp1, c, c)) != MP_OKAY) {
      goto ERR;
   }

ERR:
   mp_clear_multi(&w0, &w1, &w2, &w3, &w4,
                  &a0, &a1, &a2, &b0, &b1,
                  &b2, &tmp1, &tmp2, NULL);
   return res;
}


/*
Evaluation at {0,-2,1,-1,1/0}

M. Bodrato, "Towards Optimal Toom-Cook Multiplication for Univariate and Multivariate
Polynomials in Characteristic 2 and 0", "WAIFI'07 proceedings" (C.Carlet and B.Sunar, eds.)
Springer, Madrid, Spain, June 2007, pp. 116-133.

M. Bodrato, A. Zanoni, "Integer and Polynomial Multiplication: Towards Optimal Toom-Cook
Matrices", "Proceedings of the ISSAC 2007 conference", pp. 17-24
ACM press, Waterloo, Ontario, Canada, July 29-August 1, 2007

The following paper is behind a paywall and was hard to get.

M. Bodrato, "High degree Toom'n'half for balanced and unbalanced multiplication",
"Proceedings of the 20th IEEE symposium on Computer Arithmetic", pp 15-22, Tuebingen,
Germany, 2011
*/
int mp_toom_mul_new(mp_int *a, mp_int *b, mp_int *c){
   mp_int a0, a1, a2, b0, b1, b2, w0, w1, w2, w3, w4;

   int e = MP_OKAY;
   int B, count;

   B = ((MAX(a->used, b->used)) / 3);

   if (MIN(a->used, b->used) < TOOM_MUL_CUTOFF) {
      if ((e = mp_mul(a, b, c)) != MP_OKAY) {
         return e;
      }
      return MP_OKAY;
   }

   if ((e = mp_init_multi(&w0, &w1, &w2, &w3, &w4, NULL)) != MP_OKAY) {
      goto ERR0;
   }

   if ((e = mp_init_size(&a0, B)) != MP_OKAY) {
      goto ERRa0;
   }
   if ((e = mp_init_size(&a1, B)) != MP_OKAY) {
      goto ERRa1;
   }
   if ((e = mp_init_size(&a2, B)) != MP_OKAY) {
      goto ERRa2;
   }

   if ((e = mp_init_size(&b0, B)) != MP_OKAY) {
      goto ERRb0;
   }
   if ((e = mp_init_size(&b1, B)) != MP_OKAY) {
      goto ERRb1;
   }
   if ((e = mp_init_size(&b2, B)) != MP_OKAY) {
      goto ERRb2;
   }

   for (count = 0; count < a->used; count++) {
      switch (count / B) {
      case 0:
         a0.dp[count] = a->dp[count];
         a0.used++;
         break;
      case 1:
         a1.dp[count - B] = a->dp[count];
         a1.used++;
         break;
      case 2:
         a2.dp[count - 2 * B] = a->dp[count];
         a2.used++;
         break;
      default:
         a2.dp[count - 2 * B] = a->dp[count];
         a2.used++;
         break;
      }
   }
   mp_clamp(&a0);
   mp_clamp(&a1);
   mp_clamp(&a2);
   for (count = 0; count < b->used; count++) {
      switch (count / B) {
      case 0:
         b0.dp[count] = b->dp[count];
         b0.used++;
         break;
      case 1:
         b1.dp[count - B] = b->dp[count];
         b1.used++;
         break;
      case 2:
         b2.dp[count - 2 * B] = b->dp[count];
         b2.used++;
         break;
      default:
         b2.dp[count - 2 * B] = b->dp[count];
         b2.used++;
         break;
      }
   }
   mp_clamp(&b0);
   mp_clamp(&b1);
   mp_clamp(&b2);

// w0 = a0 * b0
mp_mul(&a0,&b0,&w0);
// w4 = a2 * b2
mp_mul(&a2,&b2,&w4);

// w1 = (a2+a1+a0) * (b2+b1+b0)
mp_add(&a2,&a1,&w1);
mp_add(&w1,&a0,&w1);

mp_add(&b2,&b1,&w2);
mp_add(&w2,&b0,&w2);

mp_mul(&w1,&w2,&w1);

// w2 = (4*a2+2*a1+a0) * (4*b2+2*b1+b0)
mp_mul_2d(&a2,2,&w2);
mp_add(&w2,&a0,&w2);
mp_mul_2(&a1,&a1);
mp_add(&w2,&a1,&w2);
mp_div_2(&a1,&a1);

mp_mul_2d(&b2,2,&w3);
mp_add(&w3,&b0,&w3);
mp_mul_2(&b1,&b1);
mp_add(&w3,&b1,&w3);
mp_div_2(&b1,&b1);

mp_mul(&w2,&w3,&w2);

// w3 = (a2-a1+a0) * (b2-b1+b0)
mp_sub(&a2,&a1,&w3);
mp_add(&w3,&a0,&w3);

// protect the environment, and reuse your variables!
mp_sub(&b2,&b1,&a0);
mp_add(&a0,&b0,&a0);

mp_mul(&a0,&w3,&w3);

// w2 = (w2 - w3)/3 exact division by 3
mp_sub(&w2,&w3,&w2);
mp_div_3(&w2,&w2,NULL);

// w3 = (w1 - w3)/2
mp_sub(&w1,&w3,&w3);
mp_div_2(&w3,&w3);

// w1 = w1 - w0
mp_sub(&w1,&w0,&w1);

// w2 = (w2 - w1)/2
mp_sub(&w2,&w1,&w2);
mp_div_2(&w2,&w2);

// w1 = w1 - w3 - w4
mp_sub(&w1,&w3,&w1);
mp_sub(&w1,&w4,&w1);

// w2 = w2 - 2*w4
mp_mul_2(&w4,&a0);
mp_sub(&w2,&a0,&w2);

// w3 = w3 - w2
mp_sub(&w3,&w2,&w3);

// w4*b^4+ w2*b^3+ w1*b^2+ w3*b + w0; don't just glimpse, look carefully!
mp_lshd(&w4,4 * B);
mp_lshd(&w2,3 * B);
mp_lshd(&w1,2 * B);
mp_lshd(&w3,    B);
mp_add(&w4,&w2,&w4);
mp_add(&w4,&w1,&w4);
mp_add(&w4,&w3,&w4);
mp_add(&w4,&w0,&w4);

mp_exch(&w4,c);


ERR:
ERRb2:
   mp_clear(&b2);
ERRb1:
   mp_clear(&b1);
ERRb0:
   mp_clear(&b0);

ERRa2:
   mp_clear(&a2);
ERRa1:
   mp_clear(&a1);
ERRa0:
   mp_clear(&a0);
ERR0:

   mp_clear_multi(&w0, &w1, &w2, &w3, &w4, NULL);
   return e;
}
#endif

/* $Source$ */
/* $Revision$ */
/* $Date$ */
