#include <tommath.h>
#ifdef BN_MP_DIV_NEWTON_C


static int lsh(mp_int *a, int n, mp_int *c);
static int rsh(mp_int *a, int n, mp_int *c);

static int lsh(mp_int *a, int n, mp_int *c)
{
   if (n < 0) {
      return mp_div_2d(a, -n, c, NULL);
   } else {
      return mp_mul_2d(a, n, c);
   }
}

static int rsh(mp_int *a, int n, mp_int *c)
{
   if (n < 0) {
      return mp_mul_2d(a, -n, c);
   } else {
      return mp_div_2d(a, n, c, NULL);
   }
}


int mp_div_newton(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
{

   int alen, blen, rlen, extra, err;
   mp_int t1, t2, t3, t4, ts, q, r;
   int steps, gs0, gsi, startprecision, i, *precs;

   err = MP_OKAY;
   fprintf(stderr,"a = %d, b = %d\n", a->used, b->used);
   alen = mp_count_bits(a);
   blen = mp_count_bits(b);
   rlen = alen - blen;

   // probably too much
   extra = MP_DIGIT_BIT;

   if ((err = mp_init_multi(&t1, &t2, &t3, &t4, &ts, &q, &r, NULL)) != MP_OKAY) {
      return err;
   }

   if ((err = mp_mul_2d(a, extra, &ts)) != MP_OKAY) {
      goto _ERR;
   }
   alen += extra;
   rlen += extra;

   // quite arbitrary number
   startprecision = 15;
   // stepsize = 2
   if ((err =
           mp_giantsteps(startprecision, rlen, 2, &precs, &steps)) != MP_OKAY) {
      goto _ERR;
   }
   gs0 = *precs;

   mp_set(&t1, 1);
   if ((err = mp_mul_2d(&t1, 2 * gs0, &t1)) != MP_OKAY) {
      goto _ERR;
   }
   if ((err = mp_div_2d(b, blen - gs0, &t2, NULL)) != MP_OKAY) {
      goto _ERR;
   }

   if ((err = mp_div_bz(&t1, &t2, &t1, NULL)) != MP_OKAY) {
      goto _ERR;
   }

   for (i = 0; i < steps; i++) {
      gsi = *(precs + i);
      // Adjust numerator (2^k) to new precision
      if ((err = lsh(&t1, gsi - gs0 + 1, &t3)) != MP_OKAY) {
         goto _ERR;
      }
      // Adjust denominator to new precision
      if ((err = rsh(b, blen - gsi, &t4)) != MP_OKAY) {
         goto _ERR;
      }
      // Do the squaring of the Newton-Raphson algorithm
      if ((err = mp_sqr(&t1, &t1)) != MP_OKAY) {
         goto _ERR;
      }
      // Do the multiplication of the Newton-Raphson algorithm
      if ((err = mp_mul(&t1, &t4, &t1)) != MP_OKAY) {
         goto _ERR;
      }
      // The division of N-R gets replaced by a simple shift
      if ((err = rsh(&t1, 2 * gs0, &t4)) != MP_OKAY) {
         goto _ERR;
      }
      // Do the subtraction of the Newton-Raphson algorithm
      if ((err = mp_sub(&t3, &t4, &t1)) != MP_OKAY) {
         goto _ERR;
      }
      gs0 = gsi;
   }
   free(precs);

   // the reciprocal is in t1, do the final multiplication to get the quotient
   // Adjust the numerator's precision to the precision of the denominator
   if ((err = mp_div_2d(&ts, blen, &ts, NULL)) != MP_OKAY) {
      goto _ERR;
   }
   // Do the actual multiplication N*1/D
   if ((err = mp_mul(&ts, &t1, &q)) != MP_OKAY) {
      goto _ERR;
   }
   // Divide by 2^k to get the quotient
   if ((err = mp_div_2d(&q, rlen + extra, &q, NULL)) != MP_OKAY) {
      goto _ERR;
   }
   // compute the remainder
   if ((err = mp_mul(&q, b, &t1)) != MP_OKAY) {
      goto _ERR;
   }
   if ((err = mp_sub(a, &t1, &r)) != MP_OKAY) {
      goto _ERR;
   }

   // The N_R algorithm as implemented can be off by one, correct it
   if (r.sign == MP_NEG) {
      if ((err = mp_add(&r, b, &r)) != MP_OKAY) {
         goto _ERR;
      }
      if ((err = mp_sub_d(&q, 1, &q)) != MP_OKAY) {
         goto _ERR;
      }
   } else if (mp_cmp(&r, b) == MP_GT) {

      if ((err = mp_sub(&r, b, &r)) != MP_OKAY) {
         goto _ERR;
      }
      if ((err = mp_add_d(&q, 1, &q)) != MP_OKAY) {
         goto _ERR;
      }

   }

   if (c != NULL) {
      mp_exch(&q, c);
   }
   if (d != NULL) {
      mp_exch(&r, d);
   }

_ERR:
   mp_clear_multi(&t1, &t2, &t3, &t4, &ts, &q, &r, NULL);
   return err;
}


#endif

