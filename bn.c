/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is library that provides for multiple-precision 
 * integer arithmetic as well as number theoretic functionality.
 * 
 * The library is designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with 
 * additional optimizations in place.  
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@iahu.ca, http://libtommath.iahu.ca
 */
#include "bn.h"

/* chars used in radix conversions */
static const char *s_rmap = 
  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";

#undef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#undef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))

/* init a new bigint */
int mp_init(mp_int *a)
{
    a->dp = calloc(sizeof(mp_digit), 16);
    if (a->dp == NULL) {
       return MP_MEM;
    }
    a->used  = 0;
    a->alloc = 16;
    a->sign  = MP_ZPOS;
    return MP_OKAY;
}

/* clear one (frees)  */
void mp_clear(mp_int *a)
{
   if (a->dp != NULL) {
      memset(a->dp, 0, sizeof(mp_digit) * a->alloc);
      free(a->dp);
      a->dp = NULL;
   }
}

/* grow as required */
static int mp_grow(mp_int *a, int size)
{
   int i;
   
   /* if the alloc size is smaller alloc more ram */
   if (a->alloc < size) {
      size += 16 - (size & 15);                         /* ensure its to the next multiple of 16 words */
      a->dp = realloc(a->dp, sizeof(mp_digit) * size);
      if (a->dp == NULL) {
         return MP_MEM;
      }
      i = a->alloc;
      a->alloc = size;
      
      /* zero top words */
      for (; i < size; i++) {
         a->dp[i] = 0;
      }
   }
   return MP_OKAY;
}

/* shrink a bignum */
int mp_shrink(mp_int *a)
{
   if (a->alloc != a->used) {
      if ((a->dp = realloc(a->dp, sizeof(mp_digit) * a->used)) == NULL) {
         return MP_MEM;
      }
      a->alloc = a->used;
   }
   return MP_OKAY;
}

/* trim unused digits */
static void mp_clamp(mp_int *a)
{
   while (a->used > 0 && a->dp[a->used-1] == 0) --(a->used);
   if (a->used == 0) {
      a->sign = MP_ZPOS;
   }      
}   
   
/* set to zero */
void mp_zero(mp_int *a)
{
   a->sign = MP_ZPOS;
   a->used = 0;
   memset(a->dp, 0, sizeof(mp_digit) * a->alloc);
}

/* set to a digit */
void mp_set(mp_int *a, mp_digit b)
{
   mp_zero(a);
   a->dp[0] = b & MP_MASK;
   a->used  = 1;
}

/* set a 32-bit const */
int mp_set_int(mp_int *a, unsigned long b)
{
   int res, x;
   if ((res = mp_grow(a, 32/DIGIT_BIT + 1)) != MP_OKAY) {
      return res;
   }
   mp_zero(a);
   /* set four bits at a time, simplest solution to the what if DIGIT_BIT==7 case */
   for (x = 0; x < 8; x++) {
      mp_mul_2d(a, 4, a);
      a->dp[0] |= (b>>28)&15;
      b <<= 4;
      a->used += 32/DIGIT_BIT + 1;
   }
   mp_clamp(a);
   return MP_OKAY;
}   

/* init a mp_init and grow it to a given size */
int mp_init_size(mp_int *a, int size)
{
   int res;
   
   if ((res = mp_init(a)) != MP_OKAY) {
      return res;
   }
   return mp_grow(a, size);
}

/* copy, b = a */
int mp_copy(mp_int *a, mp_int *b)
{
   int res, n;
   
   /* if dst == src do nothing */
   if (a == b || a->dp == b->dp) {
      return MP_OKAY;
   }
   
   /* grow dest */
   if ((res = mp_grow(b, a->used)) != MP_OKAY) {
      return res;
   }
   
   mp_zero(b);
   b->used = a->used;
   b->sign = a->sign;
   for (n = 0; n < a->used; n++) {
       b->dp[n] = a->dp[n];
   }
   return MP_OKAY;
}

/* creates "a" then copies b into it */
int mp_init_copy(mp_int *a, mp_int *b)
{
  int res;
  
  if ((res = mp_init(a)) != MP_OKAY) {
     return res;
  }
  return mp_copy(b, a);
}

/* b = |a| */
int mp_abs(mp_int *a, mp_int *b)
{
   int res;
   if ((res = mp_copy(a, b)) != MP_OKAY) {
      return res;
   }
   b->sign = MP_ZPOS;
   return MP_OKAY;
}

/* b = -a */
int mp_neg(mp_int *a, mp_int *b)
{
   int res;
   if ((res = mp_copy(a, b)) != MP_OKAY) {
      return res;
   }
   b->sign = (a->sign == MP_ZPOS) ? MP_NEG : MP_ZPOS;
   return MP_OKAY;
}


/* compare maginitude of two ints (unsigned) */
int mp_cmp_mag(mp_int *a, mp_int *b) 
{
   int n;

   /* compare based on # of non-zero digits */   
   if (a->used > b->used) {
      return MP_GT;
   } else if (a->used < b->used) {
      return MP_LT;
   }
   
   /* compare based on digits  */
   for (n = a->used - 1; n >= 0; n--) {
       if (a->dp[n] > b->dp[n]) {
          return MP_GT;
       } else if (a->dp[n] < b->dp[n]) {
          return MP_LT;
       }
   }
   return MP_EQ;
}

/* compare two ints (signed)*/
int mp_cmp(mp_int *a, mp_int *b)
{
   /* compare based on sign */
   if (a->sign == MP_NEG && b->sign == MP_ZPOS) {
      return MP_LT;
   } else if (a->sign == MP_ZPOS && b->sign == MP_NEG) {
      return MP_GT;
   }
   return mp_cmp_mag(a, b);
}

/* compare a digit */
int mp_cmp_d(mp_int *a, mp_digit b)
{
   if (a->sign == MP_NEG) {
      return MP_LT;
   }
   
   if (a->used > 1) {
      return MP_GT;
   }
   
   if (a->dp[0] > b) {
      return MP_GT;
   } else if (a->dp[0] < b) {
      return MP_LT;
   } else {
      return MP_EQ;
   }
}

/* shift right a certain amount of digits */
void mp_rshd(mp_int *a, int b)
{
   int x;
   
   /* if b <= 0 then ignore it */
   if (b <= 0) {
      return;
   }
   
   /* if b > used then simply zero it and return */
   if (a->used < b) {
      mp_zero(a);
      return;
   }
   
   /* shift the digits down */
   for (x = 0; x < (a->used - b); x++) {
       a->dp[x] = a->dp[x + b];
   }
   
   /* zero the top digits */
   for (; x < a->used; x++) {
       a->dp[x] = 0;
   }
   mp_clamp(a);
}

/* shift left a certain amount of digits */
int mp_lshd(mp_int *a, int b)
{
   int x, res;
   
   /* if its less than zero return */
   if (b <= 0) 
      return MP_OKAY;
      
   /* grow to fit the new digits */
   if ((res = mp_grow(a, a->used + b)) != MP_OKAY) {
      return res;
   }
   
   /* increment the used by the shift amount than copy upwards */
   a->used += b;
   for (x = a->used-1; x >= b; x--) {
       a->dp[x] = a->dp[x - b];
   }
   
   /* zero the lower digits */
   for (x = 0; x < b; x++) {
       a->dp[x] = 0;
   }
   mp_clamp(a);
   return MP_OKAY;
}

/* calc a value mod 2^b */
int mp_mod_2d(mp_int *a, int b, mp_int *c)
{
   int x, res;
   
   /* if b is <= 0 then zero the int */
   if (b <= 0) {
      mp_zero(c);
      return MP_OKAY;
   }
   
   /* if the modulus is larger than the value than return */
   if (b > (int)(a->used * DIGIT_BIT)) {
      return mp_copy(a, c);
   }
   
   /* copy */
   if ((res = mp_copy(a, c)) != MP_OKAY) {
      return res;
   }

   /* zero digits above */
   for (x = (b/DIGIT_BIT) + ((b % DIGIT_BIT) == 0 ? 0 : 1); x < c->used; x++) {
       c->dp[x] = 0;
   }
   /* clear the digit that is not completely outside/inside the modulus */
   c->dp[b/DIGIT_BIT] &= (mp_digit)((((mp_digit)1)<<(b % DIGIT_BIT)) - ((mp_digit)1));
   mp_clamp(c);
   return MP_OKAY;
}
   
/* shift right by a certain bit count (store quotient in c, remainder in d) */
int  mp_div_2d(mp_int *a, int b, mp_int *c, mp_int *d)
{
   mp_digit D, r, rr;
   int x, res;
   mp_int t;
   
   if ((res = mp_init(&t)) != MP_OKAY) {
      return res;
   }
   
   if (d != NULL) {
      if ((res = mp_mod_2d(a, b, &t)) != MP_OKAY) {
         mp_clear(&t);
         return res;
      }
   }
   
   /* copy */
   if ((res = mp_copy(a, c)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   
   /* shift by as many digits in the bit count */
   mp_rshd(c, b/DIGIT_BIT);
   
   /* shift any bit count < DIGIT_BIT */
   D = (mp_digit)(b % DIGIT_BIT);
   if (D != 0) {
      r = 0;
      for (x = c->used - 1; x >= 0; x--) {
          rr = c->dp[x] & ((mp_digit)((1U<<D)-1U));
          c->dp[x] = (c->dp[x] >> D) | (r << (DIGIT_BIT-D));
          r  = rr;
      }
   }
   mp_clamp(c);
   if (d != NULL) {
      res = mp_copy(&t, d);
   } else {
      res = MP_OKAY;
   }
   mp_clear(&t);
   return MP_OKAY;
}

/* shift left by a certain bit count */
int mp_mul_2d(mp_int *a, int b, mp_int *c)
{
   mp_digit d, r, rr;
   int x, res;
   
   /* copy */
   if ((res = mp_copy(a, c)) != MP_OKAY) {
      return res;
   }

   if ((res = mp_grow(c, c->used + b/DIGIT_BIT + 1)) != MP_OKAY) {
      return res;
   }
   
   /* shift by as many digits in the bit count */
   if ((res = mp_lshd(c, b/DIGIT_BIT)) != MP_OKAY) {
      return res;
   }
   c->used = c->alloc;
   
   /* shift any bit count < DIGIT_BIT */
   d = (mp_digit)(b % DIGIT_BIT);   
   if (d != 0) {
      r = 0;
      for (x = 0; x < c->used; x++) {
          rr = (c->dp[x] >> (DIGIT_BIT - d)) & ((mp_digit)((1U<<d)-1U));
          c->dp[x] = ((c->dp[x] << d) | r) & MP_MASK;
          r  = rr;
      }
   }
   mp_clamp(c);
   return MP_OKAY;
}   

/* b = a/2 */
int mp_div_2(mp_int *a, mp_int *b)
{
   mp_digit r, rr;
   int x, res;

   /* copy */
   if ((res = mp_copy(a, b)) != MP_OKAY) {
      return res;
   }
   
   r = 0;
   for (x = b->used - 1; x >= 0; x--) {
       rr = b->dp[x] & 1;
       b->dp[x] = (b->dp[x] >> 1) | (r << (DIGIT_BIT-1));
       r  = rr;
   }
   mp_clamp(b);
   return MP_OKAY;
}

/* b = a*2 */
int mp_mul_2(mp_int *a, mp_int *b)
{
   mp_digit r, rr;
   int x, res;
   
   /* copy */
   if ((res = mp_copy(a, b)) != MP_OKAY) {
      return res;
   }

   if ((res = mp_grow(b, b->used + 1)) != MP_OKAY) {
      return res;
   }
   b->used = b->alloc;
   
   /* shift any bit count < DIGIT_BIT */
   r = 0;
   for (x = 0; x < b->used; x++) {
       rr = (b->dp[x] >> (DIGIT_BIT - 1)) & 1;
       b->dp[x] = ((b->dp[x] << 1) | r) & MP_MASK;
       r  = rr;
   }
   mp_clamp(b);
   return MP_OKAY;
}

/* low level addition */
static int s_mp_add(mp_int *a, mp_int *b, mp_int *c)
{
   mp_int t, *x;
   int res, min, max, i;
   mp_digit u;
   
   /* find sizes */
   if (a->used > b->used) {
      min = b->used;
      max = a->used;
      x   = a;
   } else if (a->used < b->used) {
      min = a->used;
      max = b->used;
      x   = b;
   } else {
      min = max = a->used;
      x = NULL;
   }
   
   /* init result */
   if ((res = mp_init_size(&t, max+1)) != MP_OKAY) {
      return res;
   }
   t.used = max+1;

   /* add digits from lower part */
   u = 0;
   for (i = 0; i < min; i++) {
       t.dp[i] = a->dp[i] + b->dp[i] + u;
       u = (t.dp[i] >> DIGIT_BIT) & 1;
       t.dp[i] &= MP_MASK;
   }
   
   /* now copy higher words if any */
   if (min != max) {
      for (; i < max; i++) { 
         t.dp[i] = x->dp[i] + u;
         u = (t.dp[i] >> DIGIT_BIT) & 1;
         t.dp[i] &= MP_MASK;
      }
   }
   
   /* add carry */
   t.dp[i] = u;

   mp_clamp(&t);   
   if ((res = mp_copy(&t, c)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   mp_clear(&t);
   return MP_OKAY;       
}

/* low level subtraction (assumes a > b) */
static int s_mp_sub(mp_int *a, mp_int *b, mp_int *c)
{
   mp_int t;
   int res, min, max, i;
   mp_digit u;
   
   /* find sizes */
   min = b->used;
   max = a->used;
   
   /* init result */
   if ((res = mp_init_size(&t, max+1)) != MP_OKAY) {
      return res;
   }
   t.used = max+1;
   
   /* sub digits from lower part */
   u = 0;
   for (i = 0; i < min; i++) {
       t.dp[i] = a->dp[i] - (b->dp[i] + u);
       u = (t.dp[i] >> DIGIT_BIT) & 1;
       t.dp[i] &= MP_MASK;
   }
   
   /* now copy higher words if any */
   if (min != max) {
      for (; i < max; i++) { 
         t.dp[i] = a->dp[i] - u;
         u = (t.dp[i] >> DIGIT_BIT) & 1;
         t.dp[i] &= MP_MASK;
      }
   }
   
   mp_clamp(&t);   
   if ((res = mp_copy(&t, c)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   
   mp_clear(&t);
   return MP_OKAY;       
}

/* low level multiplication */
#define s_mp_mul(a, b, c) s_mp_mul_digs(a, b, c, (a)->used + (b)->used + 1)

/* fast multiplier  */
/* multiplies |a| * |b| and only computes upto digs digits of result */
static int fast_s_mp_mul_digs(mp_int *a, mp_int *b, mp_int *c, int digs)
{
   mp_int t;
   int res, pa, pb, ix, iy;
   mp_word W[512], *_W;
   mp_digit tmpx, *tmpt, *tmpy;
   
   if ((res = mp_init_size(&t, digs)) != MP_OKAY) {
      return res;
   }
   t.used = digs;
   
   /* clear temp buf */
   memset(W, 0, digs*sizeof(mp_word));
   
   pa = a->used;
   for (ix = 0; ix < pa; ix++) {
       pb = MIN(b->used, digs - ix);
       tmpx = a->dp[ix];
       tmpt = &(t.dp[ix]);
       tmpy = b->dp;
       _W   = &(W[ix]);
       for (iy = 0; iy < pb; iy++) {
           *_W++ += ((mp_word)tmpx) * ((mp_word)*tmpy++);
       }
   }
   
   /* now convert the array W downto what we need */
   for (ix = 1; ix < digs; ix++) {
       W[ix]      = W[ix] + (W[ix-1] >> ((mp_word)DIGIT_BIT));
       t.dp[ix-1] = W[ix-1] & ((mp_word)MP_MASK);
   }
   t.dp[digs-1]   = W[digs-1] & ((mp_word)MP_MASK);
  
   mp_clamp(&t);
   if ((res = mp_copy(&t, c)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   mp_clear(&t);
   
   return MP_OKAY;
}

/* multiplies |a| * |b| and only computes upto digs digits of result */
static int s_mp_mul_digs(mp_int *a, mp_int *b, mp_int *c, int digs)
{
   mp_int t;
   int res, pa, pb, ix, iy;
   mp_digit u;
   mp_word r;
   mp_digit tmpx, *tmpt, *tmpy;
   
   /* can we use the fast multiplier? */
   if ((digs < 512) && digs < (1<<( (CHAR_BIT*sizeof(mp_word)) - (2*DIGIT_BIT)))) {
      return fast_s_mp_mul_digs(a,b,c,digs);
   }  
   
   if ((res = mp_init_size(&t, digs)) != MP_OKAY) {
      return res;
   }
   t.used = digs;
   
   pa = a->used;
   for (ix = 0; ix < pa; ix++) {
       u = 0;
       pb = MIN(b->used, digs - ix);
       tmpx = a->dp[ix];
       tmpt = &(t.dp[ix]);
       tmpy = b->dp;
       for (iy = 0; iy < pb; iy++) {
           r       = ((mp_word)*tmpt) + ((mp_word)tmpx) * ((mp_word)*tmpy++) + ((mp_word)u);
           *tmpt++ = (mp_digit)(r & ((mp_word)MP_MASK));
           u       = (mp_digit)(r >> ((mp_word)DIGIT_BIT));
       }
       if (ix+iy<digs)
          *tmpt = u;
   }
   
   mp_clamp(&t);
   if ((res = mp_copy(&t, c)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   mp_clear(&t);
   
   return MP_OKAY;
}

static int fast_s_mp_mul_high_digs(mp_int *a, mp_int *b, mp_int *c, int digs)
{
   mp_int t;
   int res, pa, pb, ix, iy;
   mp_word W[512], *_W;
   mp_digit tmpx, *tmpt, *tmpy;
   
   if ((res = mp_init_size(&t, a->used + b->used + 1)) != MP_OKAY) {
      return res;
   }
   t.used = a->used + b->used + 1;
   
   pa = a->used;
   pb = b->used;
   memset(&W[digs], 0, (pa + pb + 1 - digs) * sizeof(mp_word));
   for (ix = 0; ix < pa; ix++) {
       tmpx = a->dp[ix];
       tmpt = &(t.dp[digs]);
       tmpy = b->dp + (digs - ix);
       _W   = &(W[digs]);
       for (iy = digs - ix; iy < pb; iy++) {
           *_W++ += ((mp_word)tmpx) * ((mp_word)*tmpy++);
       }
   }
   
   /* now convert the array W downto what we need */
   for (ix = digs+1; ix < (pa+pb+1); ix++) {
       W[ix]      = W[ix] + (W[ix-1] >> ((mp_word)DIGIT_BIT));
       t.dp[ix-1] = W[ix-1] & ((mp_word)MP_MASK);
   }
   t.dp[(pa+pb+1)-1]   = W[(pa+pb+1)-1] & ((mp_word)MP_MASK);
   
   
   mp_clamp(&t);
   if ((res = mp_copy(&t, c)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   mp_clear(&t);
   
   return MP_OKAY;
}

/* multiplies |a| * |b| and does not compute the lower digs digits 
 * [meant to get the higher part of the product]
 */
static int s_mp_mul_high_digs(mp_int *a, mp_int *b, mp_int *c, int digs)
{
   mp_int t;
   int res, pa, pb, ix, iy;
   mp_digit u;
   mp_word r;
   mp_digit tmpx, *tmpt, *tmpy;
   
   /* can we use the fast multiplier? */
   if (((a->used + b->used + 1) < 512) && MAX(a->used, b->used) < (1<<( (CHAR_BIT*sizeof(mp_word)) - (2*DIGIT_BIT)))) {
      return fast_s_mp_mul_high_digs(a,b,c,digs);
   }  

   if ((res = mp_init_size(&t, a->used + b->used + 1)) != MP_OKAY) {
      return res;
   }
   t.used = a->used + b->used + 1;
   
   pa = a->used;
   pb = b->used;
   for (ix = 0; ix < pa; ix++) {
       u = 0;
       tmpx = a->dp[ix];
       tmpt = &(t.dp[digs]);
       tmpy = b->dp + (digs - ix);
       for (iy = digs - ix; iy < pb; iy++) {
           r       = ((mp_word)*tmpt) + ((mp_word)tmpx) * ((mp_word)*tmpy++) + ((mp_word)u);
           *tmpt++ = (mp_digit)(r & ((mp_word)MP_MASK));
           u       = (mp_digit)(r >> ((mp_word)DIGIT_BIT));
       }
       *tmpt = u;
   }
   mp_clamp(&t);
   if ((res = mp_copy(&t, c)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   mp_clear(&t);
   
   return MP_OKAY;
}

/* fast squaring */
static int fast_s_mp_sqr(mp_int *a, mp_int *b)
{
   mp_int t;
   int res, ix, iy, pa;
   mp_word  W[512], *_W;
   mp_digit tmpx, *tmpy;

   pa = a->used;
   if ((res = mp_init_size(&t, pa + pa + 1)) != MP_OKAY) {
      return res;
   }
   t.used = pa + pa + 1;
   
   /* zero temp buffer */
   memset(W, 0, (pa+pa+1)*sizeof(mp_word));
   
   for (ix = 0; ix < pa; ix++) {
       W[ix+ix]   += ((mp_word)a->dp[ix]) * ((mp_word)a->dp[ix]);
	   tmpx = a->dp[ix];
	   tmpy = &(a->dp[ix+1]);
	   _W   = &(W[ix+ix+1]);
	   for (iy = ix + 1; iy < pa; iy++) {
	       *_W++ += ((mp_word)tmpx) * ((mp_word)*tmpy++) << ((mp_word)1);
       }
   }
   for (ix = 1; ix < (pa+pa+1); ix++) {
       W[ix]      = W[ix] + (W[ix-1] >> ((mp_word)DIGIT_BIT));
       t.dp[ix-1] = W[ix-1] & ((mp_word)MP_MASK);
   }
   t.dp[(pa+pa+1)-1]   = W[(pa+pa+1)-1] & ((mp_word)MP_MASK);
   
   mp_clamp(&t);
   if ((res = mp_copy(&t, b)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   mp_clear(&t);
   return MP_OKAY;
}

/* low level squaring, b = a*a */
static int s_mp_sqr(mp_int *a, mp_int *b)
{
   mp_int t;
   int res, ix, iy, pa;
   mp_word  r, u;
   mp_digit tmpx, *tmpt;
   
   /* can we use the fast multiplier? */
   if (((a->used * 2 + 1) < 512) && a->used < (1<<( (CHAR_BIT*sizeof(mp_word)) - (2*DIGIT_BIT) - 1))) {
      return fast_s_mp_sqr(a,b);
   }  

   pa = a->used;
   if ((res = mp_init_size(&t, pa + pa + 1)) != MP_OKAY) {
      return res;
   }
   t.used = pa + pa + 1;
   
   for (ix = 0; ix < pa; ix++) {
       r           = ((mp_word)t.dp[ix+ix]) + ((mp_word)a->dp[ix]) * ((mp_word)a->dp[ix]);
       t.dp[ix+ix] = (mp_digit)(r & ((mp_word)MP_MASK));
	   u           = (r >> ((mp_word)DIGIT_BIT));
	   tmpx = a->dp[ix];
	   tmpt = &(t.dp[ix+ix+1]);
	   for (iy = ix + 1; iy < pa; iy++) {
	       r           = ((mp_word)tmpx) * ((mp_word)a->dp[iy]);
	       r           = ((mp_word)*tmpt) + r + r + ((mp_word)u);
           *tmpt++     = (mp_digit)(r & ((mp_word)MP_MASK));
    	   u           = (r >> ((mp_word)DIGIT_BIT));
       }
       r           = ((mp_word)*tmpt) + u;
       *tmpt       = (mp_digit)(r & ((mp_word)MP_MASK));
  	   u           = (r >> ((mp_word)DIGIT_BIT));
       /* propagate upwards */
       ++tmpt;
       while (u != ((mp_word)0)) { 
          r                = ((mp_word)*tmpt) + ((mp_word)1);
          *tmpt++          = (mp_digit)(r & ((mp_word)MP_MASK));
          u                = (r >> ((mp_word)DIGIT_BIT));
       }
   }
   
   mp_clamp(&t);
   if ((res = mp_copy(&t, b)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   mp_clear(&t);
   return MP_OKAY;
}

/* high level addition (handles signs) */
int mp_add(mp_int *a, mp_int *b, mp_int *c)
{
   int sa, sb, res;
   
   sa = a->sign;
   sb = b->sign;

   /* handle four cases */
   if (sa == MP_ZPOS && sb == MP_ZPOS) {
      /* both positive */
      res = s_mp_add(a, b, c);
      c->sign = MP_ZPOS;
   } else if (sa == MP_ZPOS && sb == MP_NEG) {
      /* a + -b == a - b, but if b>a then we do it as -(b-a) */
      if (mp_cmp_mag(a, b) == MP_LT) {
         res = s_mp_sub(b, a, c);
         c->sign = MP_NEG;
      } else {
         res = s_mp_sub(a, b, c);
         c->sign = MP_ZPOS;
      }
   } else if (sa == MP_NEG && sb == MP_ZPOS) {
      /* -a + b == b - a, but if a>b then we do it as -(a-b) */
      if (mp_cmp_mag(a, b) == MP_GT) {
         res = s_mp_sub(a, b, c);
         c->sign = MP_NEG;
      } else {
         res = s_mp_sub(b, a, c);
         c->sign = MP_ZPOS;
      }
   } else {
      /* -a + -b == -(a + b) */
      res = s_mp_add(a, b, c);
      c->sign = MP_NEG;
   }
   return res;
}

/* high level subtraction (handles signs) */
int mp_sub(mp_int *a, mp_int *b, mp_int *c)
{
   int sa, sb, res;
   
   sa = a->sign;
   sb = b->sign;

   /* handle four cases */
   if (sa == MP_ZPOS && sb == MP_ZPOS) {
      /* both positive, a - b, but if b>a then we do -(b - a) */
      if (mp_cmp_mag(a, b) == MP_LT) {
         /* b>a */
         res = s_mp_sub(b, a, c);
         c->sign = MP_NEG;
      } else {
         res = s_mp_sub(a, b, c);
         c->sign = MP_ZPOS;
      }
   } else if (sa == MP_ZPOS && sb == MP_NEG) {
      /* a - -b == a + b  */
      res = s_mp_add(a, b, c);
      c->sign = MP_ZPOS;
   } else if (sa == MP_NEG && sb == MP_ZPOS) {
      /* -a - b == -(a + b) */
      res = s_mp_add(a, b, c);
      c->sign = MP_NEG;
   } else {
      /* -a - -b == b - a, but if a>b == -(a - b) */
      if (mp_cmp_mag(a, b) == MP_GT) {
         res = s_mp_sub(a, b, c);
         c->sign = MP_NEG;
      } else {
         res = s_mp_sub(b, a, c);
         c->sign = MP_ZPOS;
      }
   }
   return res;
}

/* c = |a| * |b| using Karatsuba */
static int mp_karatsuba_mul(mp_int *a, mp_int *b, mp_int *c)
{
   mp_int x0, x1, y0, y1, t1, t2, x0y0, x1y1;
   int B, err, neg, x;

   err = MP_MEM;

   /* min # of digits */
   B = MIN(a->used, b->used);
 
   /* now divide in two */
   B = B/2;

   /* init copy all the temps */
   if (mp_init_size(&x0, B) != MP_OKAY) goto ERR;
   if (mp_init_size(&x1, a->used - B) != MP_OKAY) goto X0;
   if (mp_init_size(&y0, B) != MP_OKAY) goto X1;
   if (mp_init_size(&y1, b->used - B) != MP_OKAY) goto Y0;

   /* init temps */
   if (mp_init(&t1) != MP_OKAY)         goto Y1;
   if (mp_init(&t2) != MP_OKAY)         goto T1;
   if (mp_init(&x0y0) != MP_OKAY)       goto T2;
   if (mp_init(&x1y1) != MP_OKAY)       goto X0Y0;

   /* now shift the digits */
   x0.sign = x1.sign = a->sign;
   y0.sign = y1.sign = b->sign;
   
   x0.used = y0.used = B;
   x1.used = a->used - B;
   y1.used = b->used - B;
   
   for (x = 0; x < B; x++) {
      x0.dp[x] = a->dp[x];
      y0.dp[x] = b->dp[x];
   }
   for (x = B; x < a->used; x++) {
      x1.dp[x-B] = a->dp[x];
   }
   for (x = B; x < b->used; x++) {
      y1.dp[x-B] = b->dp[x];
   }
   
   mp_clamp(&x0);
   mp_clamp(&x1);
   mp_clamp(&y0);
   mp_clamp(&y1);
   
   /* now calc the products x0y0 and x1y1 */
   if (mp_mul(&x0, &y0, &x0y0) != MP_OKAY) goto X1Y1;   /* x0y0 = x0*y0 */
   if (mp_mul(&x1, &y1, &x1y1) != MP_OKAY) goto X1Y1;   /* x1y1 = x1*y1 */

   /* now calc x1-x0 and y1-y0 */
   if (mp_sub(&x1, &x0, &t1) != MP_OKAY) goto X1Y1;               /* t1 = x1 - x0 */
   if (mp_sub(&y1, &y0, &t2) != MP_OKAY) goto X1Y1;               /* t2 = y1 - y0 */
   neg = (t1.sign == t2.sign) ? MP_ZPOS : MP_NEG;
   if (mp_mul(&t1, &t2, &t1) != MP_OKAY) goto X1Y1;               /* t1 = (x1 - x0) * (y1 - y0) */
   t1.sign = neg;

   /* add x0y0 */
   if (mp_add(&x0y0, &x1y1, &t2) != MP_OKAY) goto X1Y1;           /* t2 = x0y0 + x1y1 */
   if (mp_sub(&t2, &t1, &t1) != MP_OKAY) goto X1Y1;               /* t1 = x0y0 + x1y1 - (x1-x0)*(y1-y0) */

   /* shift by B */
   if (mp_lshd(&t1, B) != MP_OKAY) goto X1Y1;                   /* t1 = (x0y0 + x1y1 - (x1-x0)*(y1-y0))<<B */
   if (mp_lshd(&x1y1, B*2) != MP_OKAY) goto X1Y1;               /* x1y1 = x1y1 << 2*B */

   if (mp_add(&x0y0, &t1, &t1) != MP_OKAY) goto X1Y1;             /* t1 = x0y0 + t1 */
   if (mp_add(&t1, &x1y1, &t1) != MP_OKAY) goto X1Y1;             /* t1 = x0y0 + t1 + x1y1 */

   err = mp_copy(&t1, c);

X1Y1: mp_clear(&x1y1);
X0Y0: mp_clear(&x0y0);
T2  : mp_clear(&t2);
T1  : mp_clear(&t1);
Y1  : mp_clear(&y1);
Y0  : mp_clear(&y0);
X1  : mp_clear(&x1);
X0  : mp_clear(&x0);
ERR :
    return err;
}

/* high level multiplication (handles sign) */
int mp_mul(mp_int *a, mp_int *b, mp_int *c)
{
   int res, neg;
   neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;
   if (MIN(a->used, b->used) > KARATSUBA_MUL_CUTOFF) {
      res = mp_karatsuba_mul(a, b, c);
   } else {
      res = s_mp_mul(a, b, c);
   }
   c->sign = neg;
   return res;
}

/* Karatsuba squaring, computes b = a*a */
static int mp_karatsuba_sqr(mp_int *a, mp_int *b)
{
   mp_int x0, x1, t1, t2, x0x0, x1x1;
   int B, err;

   err = MP_MEM;

   /* min # of digits */
   B = a->used;

   /* now divide in two */
   B = B/2;

   /* init copy all the temps */
   if (mp_init_copy(&x0, a) != MP_OKAY) goto ERR;
   if (mp_init_copy(&x1, a) != MP_OKAY) goto X0;

   /* init temps */
   if (mp_init(&t1) != MP_OKAY)         goto X1;
   if (mp_init(&t2) != MP_OKAY)         goto T1;
   if (mp_init(&x0x0) != MP_OKAY)       goto T2;
   if (mp_init(&x1x1) != MP_OKAY)       goto X0X0;

   /* now shift the digits */
   mp_mod_2d(&x0, B*DIGIT_BIT, &x0);
   mp_rshd(&x1, B);

   /* now calc the products x0*x0 and x1*x1 */
   if (s_mp_sqr(&x0, &x0x0) != MP_OKAY) goto X1X1;   /* x0x0 = x0*x0 */
   if (s_mp_sqr(&x1, &x1x1) != MP_OKAY) goto X1X1;   /* x1x1 = x1*x1 */

   /* now calc x1-x0 and y1-y0 */
   if (mp_sub(&x1, &x0, &t1) != MP_OKAY) goto X1X1;               /* t1 = x1 - x0 */
   if (s_mp_sqr(&t1, &t1) != MP_OKAY) goto X1X1;                  /* t1 = (x1 - x0) * (y1 - y0) */

   /* add x0y0 */
   if (mp_add(&x0x0, &x1x1, &t2) != MP_OKAY) goto X1X1;           /* t2 = x0y0 + x1y1 */
   if (mp_sub(&t2, &t1, &t1) != MP_OKAY) goto X1X1;               /* t1 = x0y0 + x1y1 - (x1-x0)*(y1-y0) */

   /* shift by B */
   if (mp_lshd(&t1, B) != MP_OKAY) goto X1X1;                     /* t1 = (x0y0 + x1y1 - (x1-x0)*(y1-y0))<<B */
   if (mp_lshd(&x1x1, B*2) != MP_OKAY) goto X1X1;                 /* x1y1 = x1y1 << 2*B */

   if (mp_add(&x0x0, &t1, &t1) != MP_OKAY) goto X1X1;             /* t1 = x0y0 + t1 */
   if (mp_add(&t1, &x1x1, &t1) != MP_OKAY) goto X1X1;             /* t1 = x0y0 + t1 + x1y1 */

   err = mp_copy(&t1, b);
X1X1: mp_clear(&x1x1);
X0X0: mp_clear(&x0x0);
T2  : mp_clear(&t2);
T1  : mp_clear(&t1);
X1  : mp_clear(&x1);
X0  : mp_clear(&x0);
ERR :
    return err;
}

/* computes b = a*a */
int mp_sqr(mp_int *a, mp_int *b)
{
   int res;
   if (a->used > KARATSUBA_SQR_CUTOFF) {
      res = mp_karatsuba_sqr(a, b);
   } else {
      res = s_mp_sqr(a, b);
   }
   b->sign = MP_ZPOS;
   return res;
}


/* integer signed division. c*b + d == a [e.g. a/b, c=quotient, d=remainder] */
int mp_div(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
{
   mp_int q, x, y, t1, t2;
   int res, n, t, i, norm, neg;
   
   /* is divisor zero ? */
   if (mp_iszero(b) == 1) {
      return MP_VAL;
   }
   
   /* if a < b then q=0, r = a */
   if (mp_cmp_mag(a, b) == MP_LT) {
      if (d != NULL) {
           res = mp_copy(a, d);
      } else {
           res = MP_OKAY;
      }
      if (c != NULL) {
         mp_zero(c);
      }
      return res;
   }
   
 
   if ((res = mp_init_size(&q, a->used + 2)) != MP_OKAY) {
      return res;
   }
   q.used = a->used + 2;
   
   if ((res = mp_init(&t1)) != MP_OKAY) {
      goto __Q;
   }
   
   if ((res = mp_init(&t2)) != MP_OKAY) {
      goto __T1;
   }
   
   if ((res = mp_init_copy(&x, a)) != MP_OKAY) {
      goto __T2;
   }

   if ((res = mp_init_copy(&y, b)) != MP_OKAY) {
      goto __X;
   }
   
   /* fix the sign */
   neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;
   x.sign = y.sign = MP_ZPOS;
   
   /* normalize */
   norm = 0;
   while ((y.dp[y.used-1] & (((mp_digit)1)<<(DIGIT_BIT-1))) == ((mp_digit)0)) {
      ++norm;
      if ((res = mp_mul_2d(&x, 1, &x)) != MP_OKAY) {
         goto __Y;
      }
      if ((res = mp_mul_2d(&y, 1, &y)) != MP_OKAY) {
         goto __Y;
      }
   }
   
   /* note hac does 0 based, so if used==5 then its 0,1,2,3,4, e.g. use 4 */
   n = x.used - 1;
   t = y.used - 1;

   /* step 2. while (x >= y*b^n-t) do { q[n-t] += 1; x -= y*b^{n-t} } */
   if ((res = mp_lshd(&y, n - t)) != MP_OKAY) {                            /* y = y*b^{n-t} */
      goto __Y;
   }
 
   while (mp_cmp(&x, &y) != MP_LT) {
       ++(q.dp[n - t]);
       if ((res = mp_sub(&x, &y, &x)) != MP_OKAY) {
          goto __Y;
       }
   }
   
   /* reset y by shifting it back down */
   mp_rshd(&y, n - t);
   
   /* step 3. for i from n down to (t + 1) */
   for (i = n; i >= (t + 1); i--) {
       /* step 3.1 if xi == yt then set q{i-t-1} to b-1, otherwise set q{i-t-1} to (xi*b + x{i-1})/yt */
       if (x.dp[i] == y.dp[t]) {
          q.dp[i - t - 1] = ((1UL<<DIGIT_BIT)-1UL);
       } else {
          mp_word tmp;
          tmp  = ((mp_word)x.dp[i]) << ((mp_word)DIGIT_BIT);
          tmp |= ((mp_word)x.dp[i-1]);
          tmp /= ((mp_word)y.dp[t]);
          if (tmp > (mp_word)MP_MASK) tmp = MP_MASK;
          q.dp[i - t - 1] = (mp_digit)(tmp & (mp_word)(MP_MASK));
       }
       
       /* step 3.2 while (q{i-t-1} * (yt * b + y{t-1})) > xi * b^2 + xi-1 * b + xi-2 do q{i-t-1} -= 1; */
       q.dp[i-t-1] = (q.dp[i-t-1] + 1) & MP_MASK;
       do {
          q.dp[i-t-1] = (q.dp[i-t-1] - 1) & MP_MASK;
          
          /* find left hand */
          t1.dp[0] = (t-1 < 0) ? 0 : y.dp[t-1];
          t1.dp[1] = y.dp[t];
          t1.used = 2;
          if ((res = mp_mul_d(&t1, q.dp[i-t-1], &t1)) != MP_OKAY) {
             goto __Y;
          }
          
          /* find right hand */
          t2.dp[0] = (i - 2 < 0) ? 0 : x.dp[i-2];
          t2.dp[1] = (i - 1 < 0) ? 0 : x.dp[i-1];
          t2.dp[2] = x.dp[i];
          t2.used = 3;
       } while (mp_cmp(&t1, &t2) == MP_GT);
        
       /* step 3.3 x = x - q{i-t-1} * y * b^{i-t-1} */
       if ((res = mp_mul_d(&y, q.dp[i-t-1], &t1)) != MP_OKAY) {
          goto __Y;
       }
       
       if ((res = mp_lshd(&t1, i - t - 1)) != MP_OKAY) {
          goto __Y;
       }
       
       if ((res = mp_sub(&x, &t1, &x)) != MP_OKAY) { 
          goto __Y;
       }
       
       /* step 3.4 if x < 0 then { x = x + y*b^{i-t-1}; q{i-t-1} -= 1; } */
       if (x.sign == MP_NEG && x.used != 0) {
          if ((res = mp_copy(&y, &t1)) != MP_OKAY) {
             goto __Y;
          }
          if ((res = mp_lshd(&t1, i-t-1)) != MP_OKAY) {
             goto __Y;
          }
          if ((res = mp_add(&x, &t1, &x)) != MP_OKAY) {
             goto __Y;
          }
          
          q.dp[i-t-1] = (q.dp[i-t-1] - 1UL) & MP_MASK;
       }
    }
    
    /* now q is the quotient and x is the remainder [which we have to normalize] */
    /* get sign before writing to c */
    x.sign = a->sign;
    if (c != NULL) {
       mp_clamp(&q);
       mp_copy(&q, c);
       c->sign = neg;
    }
    
    if (d != NULL) {
       mp_div_2d(&x, norm, &x, NULL);
       mp_clamp(&x);
       mp_copy(&x, d);
    }
        
   res = MP_OKAY;

__Y:   mp_clear(&y);
__X:   mp_clear(&x);
__T2:  mp_clear(&t2);
__T1:  mp_clear(&t1);
__Q:   mp_clear(&q);
   return res;   
}

/* c = a mod b, 0 <= c < b */
int mp_mod(mp_int *a, mp_int *b, mp_int *c)
{
   mp_int t;
   int res;
   
   if ((res = mp_init(&t)) != MP_OKAY) {
      return res;
   }

   if ((res = mp_div(a, b, NULL, &t)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   
   if (t.sign == MP_NEG) {
      res = mp_add(b, &t, c);
   } else {
      res = mp_copy(&t, c);
   }

   mp_clear(&t);
   return res;
}

/* single digit addition */
int mp_add_d(mp_int *a, mp_digit b, mp_int *c)
{
   mp_int t;
   int res;
   
   if ((res = mp_init(&t)) != MP_OKAY) {
      return res;
   }
   mp_set(&t, b);
   res = mp_add(a, &t, c);

   mp_clear(&t);
   return res;
}   
   
/* single digit subtraction */
int mp_sub_d(mp_int *a, mp_digit b, mp_int *c)
{
   mp_int t;
   int res;
   
   if ((res = mp_init(&t)) != MP_OKAY) {
      return res;
   }
   mp_set(&t, b);
   res = mp_sub(a, &t, c);

   mp_clear(&t);
   return res;
}

/* multiply by a digit */
int mp_mul_d(mp_int *a, mp_digit b, mp_int *c)
{
   int res, pa, ix;
   mp_word  r;
   mp_digit u;
   mp_int   t;
   
   pa = a->used;
   if ((res = mp_init_size(&t, pa + 2)) != MP_OKAY) {
      return res;
   }
   t.used = pa + 2;
   
   u = 0;
   for (ix = 0; ix < pa; ix++) {
       r = ((mp_word)u) + ((mp_word)a->dp[ix]) * ((mp_word)b);
       t.dp[ix] = (mp_digit)(r & ((mp_word)MP_MASK));
	   u        = (mp_digit)(r >> ((mp_word)DIGIT_BIT));
   }
   t.dp[ix] = u;
   
   t.sign = a->sign;
   mp_clamp(&t);
   if ((res = mp_copy(&t, c)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   mp_clear(&t);
   return MP_OKAY;
}

/* single digit division */
int mp_div_d(mp_int *a, mp_digit b, mp_int *c, mp_digit *d)
{
   mp_int t, t2;
   int res;
   
   if ((res = mp_init(&t)) != MP_OKAY) {
      return res;
   }

   if ((res = mp_init(&t2)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }

   mp_set(&t, b);
   res = mp_div(a, &t, c, &t2);

   if (d != NULL) {
      *d = t2.dp[0];
   }
   
   mp_clear(&t);
   mp_clear(&t2);
   return res;
}

int mp_mod_d(mp_int *a, mp_digit b, mp_digit *c)
{
   mp_int t, t2;
   int res;
      
   if ((res = mp_init(&t)) != MP_OKAY) {
      return res;
   }
   
   if ((res = mp_init(&t2)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   
   mp_set(&t, b);
   mp_div(a, &t, NULL, &t2);
   
   if (t2.sign == MP_NEG) {
      if ((res = mp_add_d(&t2, b, &t2)) != MP_OKAY) {
         mp_clear(&t);
         mp_clear(&t2);
         return res;
      }
   }
   *c = t2.dp[0];
   mp_clear(&t);
   mp_clear(&t2);
   return MP_OKAY;
}

int mp_expt_d(mp_int *a, mp_digit b, mp_int *c)
{
   int res, x;
   mp_int g;
   
   if ((res = mp_init_copy(&g, a)) != MP_OKAY) {
      return res;
   }
   
   /* set initial result */
   mp_set(c, 1);
   
   for (x = 0; x < (int)DIGIT_BIT; x++) {
       if ((res = mp_sqr(c, c)) != MP_OKAY) {
          mp_clear(&g);
          return res;
       }
       
       if (b & (mp_digit)(1<<(DIGIT_BIT-1))) {
          if ((res = mp_mul(c, &g, c)) != MP_OKAY) {
             mp_clear(&g);
             return res;
          }
       }
       
       b <<= 1;
   }
   
   mp_clear(&g);
   return MP_OKAY;
}

/* simple modular functions */

/* d = a + b (mod c) */
int mp_addmod(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
{
   int res;
   mp_int t;
   
   if ((res = mp_init(&t)) != MP_OKAY) { 
      return res;
   }
   
   if ((res = mp_add(a, b, &t)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   res = mp_mod(&t, c, d);
   mp_clear(&t);
   return res;
}

/* d = a - b (mod c) */
int mp_submod(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
{
   int res;
   mp_int t;
   
   if ((res = mp_init(&t)) != MP_OKAY) { 
      return res;
   }
   
   if ((res = mp_sub(a, b, &t)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   res = mp_mod(&t, c, d);
   mp_clear(&t);
   return res;
}

/* d = a * b (mod c) */
int mp_mulmod(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
{
   int res;
   mp_int t;
   
   if ((res = mp_init(&t)) != MP_OKAY) { 
      return res;
   }
   
   if ((res = mp_mul(a, b, &t)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   res = mp_mod(&t, c, d);
   mp_clear(&t);
   return res;
}

/* c = a * a (mod b) */
int mp_sqrmod(mp_int *a, mp_int *b, mp_int *c)
{
   int res;
   mp_int t;
   
   if ((res = mp_init(&t)) != MP_OKAY) { 
      return res;
   }
   
   if ((res = mp_sqr(a, &t)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   res = mp_mod(&t, b, c);
   mp_clear(&t);
   return res;
}

/* Greatest Common Divisor using the binary method [Algorithm B, page 338, vol2 of TAOCP] 
 */
int mp_gcd(mp_int *a, mp_int *b, mp_int *c)
{
   mp_int u, v, t;
   int k, res, neg;
   
   /* either zero than gcd is the largest */
   if (mp_iszero(a) == 1 && mp_iszero(b) == 0) {
      return mp_copy(b, c);
   }
   if (mp_iszero(a) == 0 && mp_iszero(b) == 1) {
      return mp_copy(a, c);
   }
   if (mp_iszero(a) == 1 && mp_iszero(b) == 1) {
      mp_set(c, 1);
      return MP_OKAY;
   }
   
   /* if both are negative they share (-1) as a common divisor */
   neg = (a->sign == b->sign) ? a->sign : MP_ZPOS;
   
   if ((res = mp_init_copy(&u, a)) != MP_OKAY) {
      return res;
   }
   
   if ((res = mp_init_copy(&v, b)) != MP_OKAY) {
      goto __U;
   }
   
   /* must be positive for the remainder of the algorithm */
   u.sign = v.sign = MP_ZPOS;
   
   if ((res = mp_init(&t)) != MP_OKAY) {
      goto __V;
   }
   
   /* B1.  Find power of two */
   k = 0;
   while ((u.dp[0] & 1) == 0 && (v.dp[0] & 1) == 0) {
       ++k;
       mp_div_2d(&u, 1, &u, NULL);
       mp_div_2d(&v, 1, &v, NULL);
   }
   
   /* B2.  Initialize */
   if ((u.dp[0] & 1) == 1) {
      if ((res = mp_copy(&v, &t)) != MP_OKAY) {
         goto __T;
      }
      t.sign = MP_NEG;
   } else {
      if ((res = mp_copy(&u, &t)) != MP_OKAY) {
         goto __T;
      }
   }
   
   do {
      /* B3 (and B4).  Halve t, if even */
      while (t.used != 0 && (t.dp[0] & 1) == 0) {
          mp_div_2d(&t, 1, &t, NULL);
      }
      
      /* B5.  if t>0 then u=t otherwise v=-t */
      if (t.used != 0 && t.sign != MP_NEG) {
         if ((res = mp_copy(&t, &u)) != MP_OKAY) {
            goto __T;
         }
      } else {
         if ((res = mp_copy(&t, &v)) != MP_OKAY) {
            goto __T;
         }
         v.sign = (v.sign == MP_ZPOS) ? MP_NEG : MP_ZPOS;
      }
      
      /* B6.  t = u - v, if t != 0 loop otherwise terminate */
      if ((res = mp_sub(&u, &v, &t)) != MP_OKAY) {
         goto __T;
      }
   } while (t.used != 0);
   
   if ((res = mp_mul_2d(&u, k, &u)) != MP_OKAY) {
      goto __T;
   }
   
   if ((res = mp_copy(&u, c)) != MP_OKAY) {
      goto __T;
   }
   c->sign = neg;
__T:   mp_clear(&t);
__V:   mp_clear(&u);
__U:   mp_clear(&v);
       
   return res;
}
         
/* computes least common multipble as a*b/(a, b) */
int mp_lcm(mp_int *a, mp_int *b, mp_int *c)
{
   int res;
   mp_int t;
   
   if ((res = mp_init(&t)) != MP_OKAY) {
      return res;
   }
   
   if ((res = mp_mul(a, b, &t)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   
   if ((res = mp_gcd(a, b, c)) != MP_OKAY) {
      mp_clear(&t);
      return res;
   }
   
   res = mp_div(&t, c, c, NULL);
   mp_clear(&t);
   return res;
}   

/* computes the modular inverse via binary extended euclidean algorithm, that is c = 1/a mod b */
int mp_invmod(mp_int *a, mp_int *b, mp_int *c)
{
   mp_int x, y, u, v, A, B, C, D;
   int res, neg;
   
   if ((res = mp_init(&x)) != MP_OKAY) {
      goto __ERR;
   }
   
   if ((res = mp_init(&y)) != MP_OKAY) {
      goto __X;
   }
   
   if ((res = mp_init(&u)) != MP_OKAY) {
      goto __Y;
   }
   
   if ((res = mp_init(&v)) != MP_OKAY) {
      goto __U;
   }
   
   if ((res = mp_init(&A)) != MP_OKAY) {
      goto __V;
   }
   
   if ((res = mp_init(&B)) != MP_OKAY) {
      goto __A;
   }
   
   if ((res = mp_init(&C)) != MP_OKAY) {
      goto __B;
   }
   
   if ((res = mp_init(&D)) != MP_OKAY) {
      goto __C;
   }
   
   /* x = a, y = b */
   if ((res = mp_copy(a, &x)) != MP_OKAY) {
      goto __D;
   }
   if ((res = mp_copy(b, &y)) != MP_OKAY) {
      goto __D;
   }
   
   if ((res = mp_abs(&x, &x)) != MP_OKAY) {
      goto __D;
   }
   
   /* 2. [modified] if x,y are both even then return an error! */
   if (mp_iseven(&x) == 1 && mp_iseven(&y) == 1) {
      res = MP_VAL;
      goto __D;
   }
   
   /* 3. u=x, v=y, A=1, B=0, C=0,D=1 */
   if ((res = mp_copy(&x, &u)) != MP_OKAY) {
      goto __D;
   }
   if ((res = mp_copy(&y, &v)) != MP_OKAY) {
      goto __D;
   }
   mp_set(&A, 1);
   mp_set(&D, 1);
   

top:   
   /* 4.  while u is even do */
   while (mp_iseven(&u) == 1) {
      /* 4.1 u = u/2 */
      if ((res = mp_div_2(&u, &u)) != MP_OKAY) {
         goto __D;
      }
      /* 4.2 if A or B is odd then */
      if (mp_iseven(&A) == 0 || mp_iseven(&B) == 0) {
         /* A = (A+y)/2, B = (B-x)/2 */
         if ((res = mp_add(&A, &y, &A)) != MP_OKAY) {
            goto __D;
         }
         if ((res = mp_sub(&B, &x, &B)) != MP_OKAY) {
            goto __D;
         }
      }
      /* A = A/2, B = B/2 */
	  if ((res = mp_div_2(&A, &A)) != MP_OKAY) {
	     goto __D;
	  }
	  if ((res = mp_div_2(&B, &B)) != MP_OKAY) {
	     goto __D;
	  }
   }
   
  
   /* 5.  while v is even do */
   while (mp_iseven(&v) == 1) {
      /* 5.1 v = v/2 */
      if ((res = mp_div_2(&v, &v)) != MP_OKAY) {
         goto __D;
      }
      /* 5.2 if C,D are even then */
      if (mp_iseven(&C) == 0 || mp_iseven(&D) == 0) {
         /* C = (C+y)/2, D = (D-x)/2 */
         if ((res = mp_add(&C, &y, &C)) != MP_OKAY) {
            goto __D;
         }
         if ((res = mp_sub(&D, &x, &D)) != MP_OKAY) {
            goto __D;
         }
      }
      /* C = C/2, D = D/2 */
	  if ((res = mp_div_2(&C, &C)) != MP_OKAY) {
	     goto __D;
	  }
	  if ((res = mp_div_2(&D, &D)) != MP_OKAY) {
	     goto __D;
	  }
   }
   
   /* 6.  if u >= v then */
   if (mp_cmp(&u, &v) != MP_LT) {
      /* u = u - v, A = A - C, B = B - D */
      if ((res = mp_sub(&u, &v, &u)) != MP_OKAY) {
         goto __D;
      }
   
      if ((res = mp_sub(&A, &C, &A)) != MP_OKAY) {
         goto __D;
      }
   
      if ((res = mp_sub(&B, &D, &B)) != MP_OKAY) {
         goto __D;
      }
   } else {
      /* v - v - u, C = C - A, D = D - B */
      if ((res = mp_sub(&v, &u, &v)) != MP_OKAY) {
         goto __D;
      }
   
      if ((res = mp_sub(&C, &A, &C)) != MP_OKAY) {
         goto __D;
      }
   
      if ((res = mp_sub(&D, &B, &D)) != MP_OKAY) {
         goto __D;
      }
   }
   
   /* if not zero goto step 4 */
   if (mp_iszero(&u) == 0) goto top;
   
   /* now a = C, b = D, gcd == g*v */
 
   /* if v != 1 then there is no inverse */
   if (mp_cmp_d(&v, 1) != MP_EQ) {
      res = MP_VAL;
      goto __D;
   }
   
   /* a is now the inverse */
   neg = a->sign;
   if (C.sign == MP_NEG) {
      res = mp_add(b, &C, c);
   } else {
      res = mp_copy(&C, c);
   }
   c->sign = neg;
   
__D:   mp_clear(&D);
__C:   mp_clear(&C);
__B:   mp_clear(&B);
__A:   mp_clear(&A);
__V:   mp_clear(&v);
__U:   mp_clear(&u);
__Y:   mp_clear(&y);
__X:   mp_clear(&x);
__ERR:
   return res;
}

/* pre-calculate the value required for Barrett reduction 
 * For a given modulus "b" it calulates the value required in "a"
 */
int mp_reduce_setup(mp_int *a, mp_int *b)
{
   int res;
   
   mp_set(a, 1);
   if ((res = mp_lshd(a, b->used * 2)) != MP_OKAY) {
      return res;
   }
   return mp_div(a, b, a, NULL);
}

/* reduces x mod m, assumes 0 < x < m^2, mu is precomputed via mp_reduce_setup */
int mp_reduce(mp_int *x, mp_int *m, mp_int *mu)
{
  mp_int   q;
  int      res, um = m->used;
  
  if((res = mp_init_copy(&q, x)) != MP_OKAY)
    return res;

  mp_rshd(&q, um - 1);       /* q1 = x / b^(k-1)  */
  
  /* according to HAC this is optimization is ok */
  if (((unsigned long)m->used) > (1UL<<(unsigned long)(DIGIT_BIT-1UL))) {
     if ((res = mp_mul(&q, mu, &q)) != MP_OKAY) {
        goto CLEANUP;
     }
  } else {
     if ((res = s_mp_mul_high_digs(&q, mu, &q, um-1)) != MP_OKAY) {
        goto CLEANUP;
     }
  }

  mp_rshd(&q, um + 1);       /* q3 = q2 / b^(k+1) */

  /* x = x mod b^(k+1), quick (no division) */
  mp_mod_2d(x, DIGIT_BIT * (um + 1), x);

  /* q = q * m mod b^(k+1), quick (no division) */
  s_mp_mul_digs(&q, m, &q, um + 1);

  /* x = x - q */
  if((res = mp_sub(x, &q, x)) != MP_OKAY)
    goto CLEANUP;

  /* If x < 0, add b^(k+1) to it */
  if(mp_cmp_d(x, 0) == MP_LT) {
    mp_set(&q, 1);
    if((res = mp_lshd(&q, um + 1)) != MP_OKAY)
      goto CLEANUP;
    if((res = mp_add(x, &q, x)) != MP_OKAY)
      goto CLEANUP;
  }

  /* Back off if it's too big */
  while(mp_cmp(x, m) != MP_LT) {
    if((res = s_mp_sub(x, m, x)) != MP_OKAY)
      break;
  }

 CLEANUP:
  mp_clear(&q);

  return res;
}

int mp_exptmod(mp_int *G, mp_int *X, mp_int *P, mp_int *Y)
{
   mp_int M[64], res, mu;
   mp_digit buf;
   int err, bitbuf, bitcpy, bitcnt, mode, digidx, x, y, z, winsize, tab[64];
   
   /* find window size */
   x = mp_count_bits(X);
        if (x <= 18)    { winsize = 2; }
   else if (x <= 84)    { winsize = 3; }
   else if (x <= 300)   { winsize = 4; }
   else if (x <= 930)   { winsize = 5; }
   else                 { winsize = 6; }
   
   /* init G array */
   for (x = 0; x < (1<<winsize); x++) {
      if ((err = mp_init(&M[x])) != MP_OKAY) {
         for (y = 0; y < x; y++) {
            mp_clear(&M[y]);
         }
         return err;
      }
   }

   /* create mu, used for Barrett reduction */
   if ((err = mp_init(&mu)) != MP_OKAY) {
      goto __M;
   }
   if ((err = mp_reduce_setup(&mu, P)) != MP_OKAY) {
      goto __MU;
   }
   
   /* create M table */
   mp_set(&M[0], 1);
   if ((err = mp_mod(G, P, &M[1])) != MP_OKAY) {
      goto __MU;
   }
   
   memset(tab, 0, sizeof(tab));
   tab[0] = tab[1] = 1;
   
   for (x = 2; x < (1 << winsize); x++) {
       if (tab[x] == 0) {
          if ((err = mp_mul(&M[x-1], &M[1], &M[x])) != MP_OKAY) {
             goto __MU;
          }
          if ((err = mp_reduce(&M[x], P, &mu)) != MP_OKAY) {
             goto __MU;
          }
          tab[x] = 1;
          
          y = x+x;
          z = x;
          while (y < (1 << winsize)) {
             tab[y] = 1;
             if ((err = mp_sqr(&M[z], &M[y])) != MP_OKAY) {
                goto __MU;
             }
             if ((err = mp_reduce(&M[y], P, &mu)) != MP_OKAY) {
                goto __MU;
             }
             z = y;
             y = y + y;
          }
       }
   }
   /* init result */
   if ((err = mp_init(&res)) != MP_OKAY) {
      goto __MU;
   }
   mp_set(&res, 1);
   
   /* set initial mode and bit cnt */
   mode   = 0;
   bitcnt = 0;
   buf    = 0;
   digidx = X->used - 1;
   bitcpy = bitbuf = 0;
   
   bitcnt = 1;
   for (;;) {
      /* grab next digit as required */
      if (--bitcnt == 0) {
         if (digidx == -1) {
            break;
         }
         buf = X->dp[digidx--];
         bitcnt = DIGIT_BIT;
      }
      
      /* grab the next msb from the exponent */
      y = (buf >> (DIGIT_BIT - 1)) & 1;
      buf <<= 1;
    
      /* if the bit is zero and mode == 0 then we ignore it 
       * These represent the leading zero bits before the first 1 bit
       * in the exponent.  Technically this opt is not required but it 
       * does lower the # of trivial squaring/reductions used
       */
      if (y == 0 && mode == 0) continue;
      
      /* if the bit is zero and mode == 1 then we square */
      if (y == 0 && mode == 1) {
         if ((err = mp_sqr(&res, &res)) != MP_OKAY) {
            goto __RES;
         }
         if ((err = mp_reduce(&res, P, &mu)) != MP_OKAY) {
            goto __RES;
         }
         continue;
      }
      
      /* else we add it to the window */
      bitbuf  |= (y<<(winsize-++bitcpy));
      mode     = 2;
      
      if (bitcpy == winsize) {
         /* ok window is filled so square as required and multiply multiply */
         /* square first */
         for (x = 0; x < winsize; x++) {
            if ((err = mp_sqr(&res, &res)) != MP_OKAY) {
               goto __RES;
            }
            if ((err = mp_reduce(&res, P, &mu)) != MP_OKAY) {
               goto __RES;
            }
         }
         
         /* then multiply */
         if ((err = mp_mul(&res, &M[bitbuf], &res)) != MP_OKAY) {
            goto __MU;
         }
         if ((err = mp_reduce(&res, P, &mu)) != MP_OKAY) {
            goto __MU;
         }
         
         /* empty window and reset */
         bitcpy = bitbuf = 0;
         mode   = 1;
      }
   }
   
   /* if bits remain then square/multiply */
   if (mode == 2 && bitcpy > 0) {
      bitbuf >>= (winsize - bitcpy);
      /* square first */
      for (x = 0; x < bitcpy; x++) {
         if ((err = mp_sqr(&res, &res)) != MP_OKAY) {
            goto __RES;
         }
         if ((err = mp_reduce(&res, P, &mu)) != MP_OKAY) {
            goto __RES;
         }
      }
         
      /* then multiply */
      if ((err = mp_mul(&res, &M[bitbuf], &res)) != MP_OKAY) {
         goto __MU;
      }
      if ((err = mp_reduce(&res, P, &mu)) != MP_OKAY) {
         goto __MU;
      }
   }
   
   err = mp_copy(&res, Y);
__RES: mp_clear(&res);
__MU : mp_clear(&mu);
__M  :
   for (x = 0; x < (1<<winsize); x++) {
      mp_clear(&M[x]);
   }
   return err;
}

/* --> radix conversion <-- */
/* reverse an array, used for radix code */
static void reverse(unsigned char *s, int len)
{
   int ix, iy;
   unsigned char t;
   
   ix = 0; 
   iy = len - 1;
   while (ix < iy) {
       t = s[ix]; s[ix] = s[iy]; s[iy] = t;
       ++ix;
       --iy;
   }
}

/* returns the number of bits in an int */
int mp_count_bits(mp_int *a)
{
   int r;
   mp_digit q;
   
   if (a->used == 0) {
      return 0;
   }
    
   r = (a->used - 1) * DIGIT_BIT;
   q = a->dp[a->used - 1];
   while (q) {
      ++r;
      q >>= ((mp_digit)1);
   }
   return r;
}

/* reads a unsigned char array, assumes the msb is stored first [big endian] */
int mp_read_unsigned_bin(mp_int *a, unsigned char *b, int c)
{
   int res, n;
  
   mp_zero(a);
   n = (c/DIGIT_BIT) + ((c % DIGIT_BIT) != 0 ? 1: 0);
   if ((res = mp_grow(a, a->used)) != MP_OKAY) {
      return res;
   }
   a->used = n;
   while (c-- > 0) {
       if ((res = mp_mul_2d(a, 8, a)) != MP_OKAY) {
          return res;
       }
       
       if (DIGIT_BIT != 7) {
           a->dp[0] |= *b++;
           a->used  += 1;
       } else {
           a->dp[0]  = (*b & MP_MASK);
           a->dp[1] |= ((*b++ >> 7U) & 1);
           a->used  += 2;
       }
   }
   mp_clamp(a);
   return MP_OKAY;
}   

/* read signed bin, big endian, first byte is 0==positive or 1==negative */
int mp_read_signed_bin(mp_int *a, unsigned char *b, int c)
{
   int res;
   
   if ((res = mp_read_unsigned_bin(a, b + 1, c - 1)) != MP_OKAY) {
      return res;
   }
   a->sign = ((b[0] == (unsigned char)0) ? MP_ZPOS : MP_NEG);
   return MP_OKAY;
}

/* store in unsigned [big endian] format */
int mp_to_unsigned_bin(mp_int *a, unsigned char *b)
{
   int x, res;
   mp_int t;
   
   if ((res = mp_init_copy(&t, a)) != MP_OKAY) {
      return res;
   }
   
   x = 0;
   while (mp_iszero(&t) == 0) {
      if (DIGIT_BIT != 7) {
         b[x++] = (unsigned char)(t.dp[0] & 255);
      } else {
         b[x++] = (unsigned char)(t.dp[0] | ((t.dp[1] & 0x01) << 7));
      }
      mp_div_2d(&t, 8, &t, NULL);
   }
   reverse(b, x);
   return MP_OKAY;
}

/* store in signed [big endian] format */
int mp_to_signed_bin(mp_int *a, unsigned char *b)
{
   int res;
   
   if ((res = mp_to_unsigned_bin(a, b+1)) != MP_OKAY) {
      return res;
   }
   b[0] = (unsigned char)((a->sign == MP_ZPOS) ? 0 : 1);
   return MP_OKAY;
}

/* get the size for an unsigned equivalent */
int mp_unsigned_bin_size(mp_int *a)
{
   return (mp_count_bits(a)/8 + ((mp_count_bits(a)&7) != 0 ? 1 : 0));
}

/* get the size for an signed equivalent */
int mp_signed_bin_size(mp_int *a)
{
   return 1 + (mp_count_bits(a)/8 + ((mp_count_bits(a)&7) != 0 ? 1 : 0));
}

/* read a string [ASCII] in a given radix */
int mp_read_radix(mp_int *a, unsigned char *str, int radix)
{
   int y, res, neg;
   
   if (radix < 2 || radix > 64) {
      return MP_VAL;
   }

   if (*str == (unsigned char)'-') {
      ++str;
      neg = MP_NEG;
   } else {
      neg = MP_ZPOS;
   }
   
   mp_zero(a);
   while (*str) {
      for (y = 0; y < 64; y++) {
          if (*str == (unsigned char)s_rmap[y]) {
             break;
          }
      }
      
      if (y < radix) {
         if ((res = mp_mul_d(a, (mp_digit)radix, a)) != MP_OKAY) {
            return res;
         }
         if ((res = mp_add_d(a, (mp_digit)y, a)) != MP_OKAY) {
            return res;
         }
      } else {
         break;
      }
      ++str;
   }
   a->sign = neg;
   return MP_OKAY;
}

/* stores a bignum as a ASCII string in a given radix (2..64) */
int mp_toradix(mp_int *a, unsigned char *str, int radix)
{
   int res, digs;
   mp_int t;
   mp_digit d;
   unsigned char *_s = str;
   
   if (radix < 2 || radix > 64) {
      return MP_VAL;
   }

   if ((res = mp_init_copy(&t, a)) != MP_OKAY) {
      return res;
   }
   
   if (t.sign == MP_NEG) { 
      ++_s;
      *str++ = '-';
      t.sign = MP_ZPOS;
   }
   
   digs = 0;
   while (mp_iszero(&t) == 0) {
       if ((res = mp_div_d(&t, (mp_digit)radix, &t, &d)) != MP_OKAY) {
          return res;
       }
       *str++ = (unsigned char)s_rmap[d];
       ++digs;
   }
   reverse(_s, digs);
   *str++ = (unsigned char)'\0';
   mp_clear(&t);
   return MP_OKAY;
}

/* returns size of ASCII reprensentation */
int mp_radix_size(mp_int *a, int radix)
{
   int res, digs;
   mp_int t;
   mp_digit d;
   
   digs = 0;

   if (radix < 2 || radix > 64) {
      return 0;
   }

   if ((res = mp_init_copy(&t, a)) != MP_OKAY) {
      return 0;
   }
   
   if (t.sign == MP_NEG) { 
      ++digs;
      t.sign = MP_ZPOS;
   }
   
   while (mp_iszero(&t) == 0) {
       if ((res = mp_div_d(&t, (mp_digit)radix, &t, &d)) != MP_OKAY) {
          return 0;
       }
       ++digs;
   }
   mp_clear(&t);
   return digs + 1;
}

