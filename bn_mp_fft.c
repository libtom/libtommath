#include <tommath.h>
#ifdef BN_MP_FFT_C
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

/* Multiplication with FHT convolution - core functions */

#ifdef  MP_28BIT


#include <math.h>

#define MP_DIGIT_SIZE (1L<<DIGIT_BIT)
#define MP_DIGIT_BIT_HALF (DIGIT_BIT>>1)
#define MP_DIGIT_HALF (1L<< MP_DIGIT_BIT_HALF )
#define MP_DIGIT_MASK (MP_DIGIT_HALF-1)

/* base two integer logarithm */
static int highbit(int n)
{
   int r=0;
   int m=n;
   while (m >>= 1) {
      r++;
   }
   return r;
}
/* Transform multiplicands into floating point numbers with half sized digits*/
int mp_dp_to_fft(mp_int *a, double **fa,
                 mp_int *b, double **fb, int *length)
{
   int length_a, length_b, length_needed, i,j, hb;
   double *fft_array_a,*fft_array_b;

   /* Check of the multiplicands happens earlier */
   length_a = a->used;
   length_b = b->used;

   /* Digits get split in half, so twice the length is needed*/
   length_needed = ((length_a + length_b))*2 ;
   /* final length must be a power of two to keep the FFTs simple */
   hb = highbit((unsigned  long) length_needed);
   /* check for the rare case that it is already a power of 2 */
   if (length_needed != 1<<hb) {
      length_needed = 1<<(hb+1);
   }
   /* Send computed length back to caller */
   *length = length_needed;

   fft_array_a = XMALLOC(sizeof(double) * length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }
   fft_array_b = XMALLOC(sizeof(double) * length_needed);
   if (fft_array_b == NULL) {
      return MP_MEM;
   }
   /* Put splitted digits in double-array, in the same order as in mp_int */

   for (i = 0,j=0; i<length_needed/2; i++,j+=2) {
      if (i < length_a) {
         fft_array_a[j]   = (double)(a->dp[i]                        & MP_DIGIT_MASK);
         fft_array_a[j+1] = (double)((a->dp[i] >> MP_DIGIT_BIT_HALF) & MP_DIGIT_MASK);
      }
      /* padding a */
      if (i >= length_a) {
         fft_array_a[j]   = 0.0;
         fft_array_a[j+1] = 0.0;
      }
      if (i < length_b) {
         fft_array_b[j]   = (double)(b->dp[i]                        & MP_DIGIT_MASK);
         fft_array_b[j+1] = (double)((b->dp[i] >> MP_DIGIT_BIT_HALF) & MP_DIGIT_MASK);
      }
      /* padding b */
      if (i >= length_b) {
         fft_array_b[j]   = 0.0;
         fft_array_b[j+1] = 0.0;
      }
   }
   /* Send the route to memory back to caller */
   *fa = fft_array_a;
   *fb = fft_array_b;
   return MP_OKAY;
}

/* same as dp_to_fft() for a single multiplicand for squaring */
int mp_dp_to_fft_single(mp_int *a, double **fa, int *length)
{
   int length_a,  length_needed, i,j, hb;
   double *fft_array_a;
   length_a = a->used;
   length_needed = (length_a * 2)*2 ;
   hb = highbit((unsigned  long) length_needed);
   if (length_needed != 1<<hb) {
      length_needed = 1<<(hb+1);
   }
   *length = length_needed;
   fft_array_a = XMALLOC(sizeof(double) * length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }
   for (i = 0,j=0; i<length_needed/2; i++,j+=2) {
      if (i < length_a) {
         fft_array_a[j]   = (double)(a->dp[i]         & MP_DIGIT_MASK);
         fft_array_a[j+1] = (double)((a->dp[i] >> MP_DIGIT_BIT_HALF) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[j]   = 0.0;
         fft_array_a[j+1] = 0.0;
      }
   }
   *fa = fft_array_a;
   return MP_OKAY;
}

int mp_fft_to_dp(double *fft_array, mp_int *a,int length)
{
   int new_length, i,j,e;
   mp_word carry,temp;

   /* Result cannot exceed length/2, hence add two */
   new_length = length;

   /* Preallocate some memory for the result. */
   if (a->alloc < new_length) {
      if ((e = mp_grow(a, new_length)) != MP_OKAY) {
         return e;
      }
   }

   /* The FFT multiplication does no carry (it's one of the tricks of it) */
   carry = 0;
   for (i=0; i<length; i++) {
      temp = carry;
      carry = 0;//printf("i  = %d\n",i);
      temp  += (mp_word)(round(fft_array[i]));
      if (temp >= MP_DIGIT_HALF) {
         carry = temp / (mp_word)MP_DIGIT_HALF;
         temp  = temp % (mp_word)MP_DIGIT_HALF;
      }
      /* memory is still expensive, not a thing to waste easily */
      fft_array[i] = (double)temp;
   }
#if __STDC_VERSION__ >= 199901L
#define NEEDS_FE_RESET 1
#include <fenv.h>
   fenv_t envp;
   /* backup of floating point environment settings */
   if (fegetenv(&envp)) return MP_VAL;
   /* Set rounding mode to "nearest". Default, but better safe than sorry */
   if (fesetround(FE_TONEAREST)) return MP_VAL;
#endif
   /* re-marry the digits */
   for (i=0,j=0; j<new_length; i++,j+=2) {
      a->dp[i]  = (mp_digit)(round(fft_array[j+1]))& MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_HALF;
      a->dp[i] |= (mp_digit)(round(fft_array[j]))  & MP_DIGIT_MASK;
      /* and count them all */
      a->used++;
   }
   if (carry) {
      a->dp[i] = carry;
      a->used++;
   }
   mp_clamp(a);
   return MP_OKAY;
}


#elif (defined MP_64BIT)

#include <math.h>
/* 60 bit long limbs with 64 bit machines, so we need a quarter (15 bits) */
#define MP_DIGIT_SIZE (1L<<DIGIT_BIT)
//#define MP_DIGIT_BIT_QUARTER (DIGIT_BIT>>2)
//#define MP_DIGIT_QUARTER (1L<< MP_DIGIT_BIT_QUARTER )
//#define MP_DIGIT_MASK (MP_DIGIT_QUARTER-1)

#define MP_DIGIT_BIT_FIFTH (DIGIT_BIT/5)
#define MP_DIGIT_FIFTH (1L<< MP_DIGIT_BIT_FIFTH )
#define MP_DIGIT_MASK (MP_DIGIT_FIFTH-1)

/* base two integer logarithm */
static int highbit(int n)
{
   int r=0;
   int m=n;
   while (m >>= 1) {
      r++;
   }
   return r;
}
/* Transform multiplicands into floating point numbers with fifth sized digits*/
int mp_dp_to_fft(mp_int *a, double **fa,
                 mp_int *b, double **fb, int *length)
{
   int length_a, length_b, length_needed, i, hb, rest;
   double *fft_array_a,*fft_array_b;

   /* Check of the multiplicands happens earlier */
   length_a = a->used;
   length_b = b->used;

   /* Digits get split in fifths, so five times the length is needed*/
   length_needed = ((length_a + length_b ))*5 ;
   /* final length must be a power of two to keep the FFTs simple */
   hb = highbit((unsigned  long) length_needed);
   /* check for the rare case that it is already a power of 2 */
   if (length_needed != 1<<hb) {
      length_needed = 1<<(hb+1);
   }
   /* Send computed length back to caller */
   *length = length_needed;

   fft_array_a = XMALLOC(sizeof(double) * length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }
   fft_array_b = XMALLOC(sizeof(double) * length_needed);
   if (fft_array_b == NULL) {
      return MP_MEM;
   }

   for (i = 0; i<length_needed/5; i++) {
      if (i < length_a) {
         fft_array_a[(5*i)]   = (double) (a->dp[i]                            & MP_DIGIT_MASK);
         fft_array_a[(5*i)+1] = (double)((a->dp[i] >>    MP_DIGIT_BIT_FIFTH)  & MP_DIGIT_MASK);
         fft_array_a[(5*i)+2] = (double)((a->dp[i] >> (2*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_a[(5*i)+3] = (double)((a->dp[i] >> (3*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_a[(5*i)+4] = (double)((a->dp[i] >> (4*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
      }
      /* padding a */
      if (i >= length_a) {
         fft_array_a[(5*i)]   = 0.0;
         fft_array_a[(5*i)+1] = 0.0;
         fft_array_a[(5*i)+2] = 0.0;
         fft_array_a[(5*i)+3] = 0.0;
         fft_array_a[(5*i)+4] = 0.0;
      }
      if (i < length_b) {
         fft_array_b[(5*i)]   = (double) (b->dp[i]                            & MP_DIGIT_MASK);
         fft_array_b[(5*i)+1] = (double)((b->dp[i] >>    MP_DIGIT_BIT_FIFTH)  & MP_DIGIT_MASK);
         fft_array_b[(5*i)+2] = (double)((b->dp[i] >> (2*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_b[(5*i)+3] = (double)((b->dp[i] >> (3*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_b[(5*i)+4] = (double)((b->dp[i] >> (4*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
      }
      /* padding b */
      if (i >= length_b) {
         fft_array_b[(5*i)]   = 0.0;
         fft_array_b[(5*i)+1] = 0.0;
         fft_array_b[(5*i)+2] = 0.0;
         fft_array_b[(5*i)+3] = 0.0;
         fft_array_b[(5*i)+4] = 0.0;
      }
   }
   // there is a small problem with divisibility of 2^n and 5, so ...
   rest = (length_needed/5)*5;
   for(i=rest;i<length_needed;i++){
      fft_array_a[i] = 0.0;
      fft_array_b[i] = 0.0;
   }
   /* Send the route to memory back to caller */
   *fa = fft_array_a;
   *fb = fft_array_b;
   return MP_OKAY;
}


/* same as dp_to_fft() for a single multiplicand for squaring */
int mp_dp_to_fft_single(mp_int *a, double **fa, int *length)
{
   int length_a,  length_needed, i, hb, rest;
   double *fft_array_a;
   length_a = a->used;
   length_needed = (length_a * 2)*5 ;
   hb = highbit((unsigned  long) length_needed);
   if (length_needed != 1<<hb) {
      length_needed = 1<<(hb+1);
   }
   *length = length_needed;
   fft_array_a = XMALLOC(sizeof(double) * length_needed);
   if (fft_array_a == NULL) {
      return MP_MEM;
   }
   for (i = 0; i<length_needed/5; i++) {
      if (i < length_a) {
         fft_array_a[(5*i)]   = (double)( a->dp[i]                            & MP_DIGIT_MASK);
         fft_array_a[(5*i)+1] = (double)((a->dp[i] >>    MP_DIGIT_BIT_FIFTH)  & MP_DIGIT_MASK);
         fft_array_a[(5*i)+2] = (double)((a->dp[i] >> (2*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_a[(5*i)+3] = (double)((a->dp[i] >> (3*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
         fft_array_a[(5*i)+4] = (double)((a->dp[i] >> (4*MP_DIGIT_BIT_FIFTH)) & MP_DIGIT_MASK);
      }
      if (i >= length_a) {
         fft_array_a[(5*i)]   = 0.0;
         fft_array_a[(5*i)+1] = 0.0;
         fft_array_a[(5*i)+2] = 0.0;
         fft_array_a[(5*i)+3] = 0.0;
         fft_array_a[(5*i)+4] = 0.0;
      }
   }
   rest = (length_needed/5)*5;
   for(i=rest;i<length_needed;i++){
      fft_array_a[i] = 0.0;
   }
   *fa = fft_array_a;
   return MP_OKAY;
}

int mp_fft_to_dp(double *fft_array, mp_int *a,int length)
{
   int new_length, i,j,e;
   mp_word carry = 0,temp;

   /* Result cannot exceed length/2, hence add two */
   new_length = length;

   /* Preallocate some memory for the result. */
   if (a->alloc < new_length) {
      if ((e = mp_grow(a, new_length)) != MP_OKAY) {
         return e;
      }
   }

   /* The FFT multiplication does no carry (it's one of the tricks of it) */

   /* Hard to paralellize because of the carry */
   for (i=0; i<length; i++) {
      temp = carry;
      carry = 0;
      temp  += (mp_word)(round(fft_array[i]));
      if (temp >= MP_DIGIT_FIFTH) {
         carry = temp / (mp_word)MP_DIGIT_FIFTH;
         temp  = temp % (mp_word)MP_DIGIT_FIFTH;
      }
      /* memory is still expensive, not a thing to waste easily */
      fft_array[i] = (double)temp;
   }

#if __STDC_VERSION__ >= 199901L
#define NEEDS_FE_RESET 1
#include <fenv.h>
   fenv_t envp;
   /* backup of floating point environment settings */
   if (fegetenv(&envp)) return MP_VAL;
   /* Set rounding mode to "nearest". Default, but better safe than sorry */
   if (fesetround(FE_TONEAREST)) return MP_VAL;
#endif


   /* re-marry the digits */
   for (i=0,j=0; j<new_length; i++,j+=5) {
      a->dp[i]  = (mp_digit)(round(fft_array[j+4]))  & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_FIFTH;
      a->dp[i] |= (mp_digit)(round(fft_array[j+3]))  & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_FIFTH;
      a->dp[i] |= (mp_digit)(round(fft_array[j+2]))  & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_FIFTH;
      a->dp[i] |= (mp_digit)(round(fft_array[j+1]))  & MP_DIGIT_MASK;
      a->dp[i] <<= MP_DIGIT_BIT_FIFTH;
      a->dp[i] |= (mp_digit)(round(fft_array[j]))    & MP_DIGIT_MASK;
      /* and count them all */
      a->used++;
   }

   if (carry) {
      a->dp[i] = carry;
      a->used++;
   }
   mp_clamp(a);
   return MP_OKAY;
}

/* ifdef MP_28bit or MP_64BIT*/
/* TODO: MP_8BIT would work, of course, but MP_16BIT is a bit large*/
#else
#error unsupported for now
#endif

/*
  The size of the L1-cache in bytes. The number here is that of the data cache
  part of an AMD Duron. The Linux kernel gives a lot of information e.g.:
    grep . /sys/devices/system/cpu/cpu0/cache/index*//*
  There is also lscpu(1) wich is easier to use.
  On Windows:
    http://msdn.microsoft.com/en-us/library/ms683194.aspx
    http://www.cpuid.com/softwares/cpu-z.htm
  Lack of access to a Mac leaves that part blank. The new MacOS is based on BSD,
  so 'dmesg' might work or
    cat /var/run/dmesg.boot | grep CPU
 */
#ifndef L1_SIZE
//#define L1_SIZE 65536
#define L1_SIZE 16384
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937511
#endif
#define TWOPI (2.0*M_PI)

/*
  The iterative version of a Hartley transform, decimation in frequency.
  For the technical details see the excellent description in Jörg Arndt's book
  "Matters Computational" available for no cost at
    http://www.jjj.de/fxt/fxtbook.pdf‎
  See pages 515 ff.

  Optimizations used here:
    - hartley_shift (p. 516 f.) which avoids a large number of trig. comput.

  In the case of computationally very expensive or even non-existant
  trigonometric functions the calculation of the variables {c,s} can be replaced
  with two square roots which are easier to implement.
  The following pseudo-code calculates the numbers needed, although in reverse
  order:
    c = 0.0;s = 1.0;
    for(i=0;i<n;i++){
      s = sqrt((1.0 - c) / 2.0);
      c = sqrt((1.0 + c) / 2.0);
    }

   Also: the trig. values are always an ordered subset out of an ordered set of
   sizeof(unsigned long)*CHAR_BIT tuples if the length of the data is a power of
   two. That set can be pre-computed and needs
     sizeof(unsigned long)*CHAR_BIT*2 * sizeof(double)*CHAR_BIT
   bits of memory. With the current 64-bit implementations (64 bit integers and
   64 bit doubles) it's 8 kibi.
 */
static void fht_dif_iterative(double *x, unsigned long n, int do_loop)
{
   unsigned long m,mh,mq;
   unsigned long i,j,k;
   double a,b,t, c,s, u,v,tmp;
   double *dp;
   for (m=n; m > 1; m >>= 1) {
      mh = m >> 1;
      mq = mh >> 1;
      t = M_PI / (double)mh;
      a = sin(0.5 * t);
      a *= 2.0 * a;
      b = sin(t);
      for (i = 0; i < n; i += m) {
         dp = x + i;
         for (j = 0, k = mh; j < mh; ++j, ++k) {
            u = dp[j];
            v = dp[k];
            dp[j] = u + v;
            dp[k] = u - v;
         }
         dp += mh;
         c = 1.0;
         s = 0.0;
         for (j = 1, k = mh - 1; j < mq; ++j, --k) {
            tmp = c;
            c -= a * c + b * s;
            s -= a * s - b * tmp;
            u = dp[j];
            v = dp[k];
            dp[j] = u * c + v * s;
            dp[k] = u * s - v * c;
         }
      }
      if (!do_loop)break;
   }
   return;
}

/*
  The recursive version of a Hartley transform, decimation in frequency.
  The trick is a kind of binary splitting: recurse until length of array
  is short enough to fit into the L1-cache.
  Idea found in FFTW3 but it is way older:
    Singleton, R. C., "On Computing the Fast Fourier Transform,"
    Communications of the ACM 10:647, 1967
  Seeking for this citation online found the software "am" at
  https://www.cfa.harvard.edu/~spaine/am/ where the same method can be found
  in https://www.cfa.harvard.edu/~spaine/am/download/src/transform.c

  The contruction of the functions here have been slightly adapted to the
  functions in "am" from above to make the actual FFT function easier replacable
  with others e.g.: those in "src/transform.c". They are legible now, too.
*/

#ifdef USE_PTHREAD_FFT_NOT
#include <pthread.h>
#ifndef FFT_DIF_NUM_THREADS
#define FFT_DIF_NUM_THREADS 1
#endif

struct fht_dif_params{
   double *x;
   unsigned long n;
   int depth;
};

static void *fht_dif_rec_threaded(void *p)
{
  unsigned long nh;
  struct fht_dif_params *params = p;
  if (params->n == 1)
    return NULL;
  if (params->n < (unsigned long) (L1_SIZE / (2 * sizeof(double)))) {
    fht_dif_iterative(params->x, params->n, 1);
    return NULL;
  }
  fht_dif_iterative(params->x, params->n, 0);
  nh = (params->n) >> 1;

  struct fht_dif_params param_one = {
        .x = (params->x),
        .n = nh,
        .depth = params->depth + 1
  };
  struct fht_dif_params param_two = {
        .x = (params->x) + nh,
        .n = nh,
        .depth = params->depth + 1
  };

  if (params->depth < FFT_DIF_NUM_THREADS) {
    pthread_t thread_one, thread_two;
    pthread_create(&thread_one, NULL, fht_dif_rec_threaded, &param_one);
    pthread_create(&thread_two, NULL, fht_dif_rec_threaded, &param_two);

    pthread_join(thread_one, NULL);
    pthread_join(thread_two, NULL);
  } else {
    fht_dif_rec_threaded(&param_one);
    fht_dif_rec_threaded(&param_two);
  }
  return NULL;
}

static void fht_dif_rec(double *x, unsigned long n){
  struct fht_dif_params param = {
        .x = x,
        .n = n,
        .depth = 0
  };
  fht_dif_rec_threaded(&param);
  return;
}
#else

static void fht_dif_rec(double *x, unsigned long n)
{
   unsigned long nh;
   if (n == 1)
      return;
   if (n < (unsigned long)(L1_SIZE / (2 * sizeof(double)))) {
      fht_dif_iterative(x, n, 1);
      return;
   }
   fht_dif_iterative(x, n, 0);
   nh = n >> 1;
   fht_dif_rec(x, nh);
   fht_dif_rec(x + nh, nh);
   return;
}
#endif

/* The iterative Hartley transform, decimation in time. Description above */
static void fht_dit_iterative(double *x, unsigned long n, int do_loop)
{
   unsigned long m, mh ,mq;
   unsigned long i,j,k;
   double a,b,t, u,v, c,s, tmp;
   double *dp;

   m = (do_loop)?2:n;
   for (; m <= n; m <<= 1) {
      mh = m >> 1;
      mq = mh >> 1;
      t = M_PI / (double)mh;
      a = sin(0.5 * t);
      a *= 2.0 * a;
      b = sin(t);
      for (i = 0; i < n; i += m) {
         dp = x + i + mh;
         c = 1.0;
         s = 0.0;
         for (j = 1, k = mh - 1; j < mq; ++j, --k) {
            tmp = c;
            c -= a * c + b * s;
            s -= a * s - b * tmp;
            u = dp[j];
            v = dp[k];
            dp[j] = u * c + v * s;
            dp[k] = u * s - v * c;
         }
         dp -= mh;
         for (j = 0, k = mh; j < mh; ++j, ++k) {
            u = dp[j];
            v = dp[k];
            dp[j] = u + v;
            dp[k] = u - v;
         }
      }
   }
   return;
}
/* The binary splitting. Description above */

#ifdef USE_PTHREAD_FFT_NOT
#include <pthread.h>
#ifndef FFT_DIT_NUM_THREADS
#define FFT_DIT_NUM_THREADS 1
#endif
struct fht_dit_params{
   double *x;
   unsigned long n;
   int depth;
};


static void *fht_dit_rec_threaded(void *p)
{
   unsigned long nh;
   struct fht_dit_params *params = p;

   if (params->n == 1)
      return NULL;
   if (params->n < (unsigned long)(L1_SIZE / (2 * sizeof(double)))) {
      fht_dit_iterative(params->x,params->n,1);
      return NULL;
   }
   nh = params->n >> 1;

  struct fht_dit_params param_one = {
        .x = (params->x),
        .n = nh,
        .depth = params->depth + 1
  };
  struct fht_dit_params param_two = {
        .x = (params->x) + nh,
        .n = nh,
        .depth = params->depth + 1
  };


  if (params->depth < FFT_DIT_NUM_THREADS) {
    pthread_t thread_one, thread_two;
    pthread_create(&thread_one, NULL, fht_dit_rec_threaded, &param_one);
    pthread_create(&thread_two, NULL, fht_dit_rec_threaded, &param_two);

    pthread_join(thread_one, NULL);
    pthread_join(thread_two, NULL);
  } else {
    fht_dit_rec_threaded(&param_one);
    fht_dit_rec_threaded(&param_two);
  }

   fht_dit_iterative(params->x,params->n,0);
   return NULL;
}


static void fht_dit_rec(double *x, unsigned long n){
  struct fht_dit_params param = {
        .x = x,
        .n = n,
        .depth = 0
  };
  fht_dit_rec_threaded(&param);
  return;
}


#else

static void fht_dit_rec(double *x, unsigned long n)
{
   unsigned long nh;

   if (n == 1)
      return;
   if (n < (unsigned long)(L1_SIZE / (2 * sizeof(double)))) {
      fht_dit_iterative(x,n,1);
      return;
   }
   nh = n >> 1;
   fht_dit_rec(x, nh);
   fht_dit_rec(x + nh, nh);
   fht_dit_iterative(x,n,0);
   return;
   return;
}
#endif

/*
  The FHT convolution from Jörg Arndt's book.
  The code looks a bit messy but only on the face of it. This method avoids
  the otherwise costly bit reversing (which is called "revbin" in J. Arndt's
  book).
*/
static void fht_conv_core(double *f, double *g,unsigned long n, double v/*=0.0*/)
{
   unsigned long nh,r,rm,k,km,tr,m;
   double xi,xj, yi,yj;
   if (v==0.0)  v = 1.0/n;

   g[0] *= (v * f[0]);
   if (n>=2) g[1] *= (v * f[1]);
   if (n<4)   return;
   v *= 0.5;
   nh = (n>>1);
   r=nh;
   rm=n-1;
   xi = f[r];
   xj = f[rm];
   yi = g[r];
   yj = g[rm];
   g[r]  = v*((xi + xj)*yi + (xi - xj)*yj);
   g[rm] = v*((-xi + xj)*yi + (xi + xj)*yj);

   k=2;
   km=n-2;
   while (k<nh) {
      rm -= nh;
      tr = r;
      r^=nh;
      for (m=(nh>>1); !((r^=m)&m); m>>=1) {
         ;
      }

      xi = f[r];
      xj = f[rm];
      yi = g[r];
      yj = g[rm];
      g[r]  = v*((xi + xj)*yi + (xi - xj)*yj);
      g[rm] = v*((-xi + xj)*yi + (xi + xj)*yj);
      --km;
      ++k;
      rm += (tr-r);
      r += nh;
      xi = f[r];
      xj = f[rm];
      yi = g[r];
      yj = g[rm];
      g[r]  = v*((xi + xj)*yi + (xi - xj)*yj);
      g[rm] = v*((-xi + xj)*yi + (xi + xj)*yj);
      --km;
      ++k;
   }
   return;

}
/* FHT auto-convolution. Description above */
static void fht_autoconv_core(double *f,unsigned long n, double v/*=0.0*/)
{
   unsigned long nh,r,rm,k,km,tr,m;
   double xi,xj, xi2, xj2,xij ;
   if (v==0.0)  v = 1.0/n;

   f[0] *= (v * f[0]);
   if (n>=2) f[1] *= (v * f[1]);
   if (n<4)   return;
   v *= 0.5;
   nh = (n>>1);
   r=nh;
   rm=n-1;
   xi = f[r];
   xj = f[rm];
   xi2 = xi*xi;
   xj2 = xj*xj;
   xij = (2*xi*xj);
   f[r]  = v*(xi2 + xij - xj2);
   f[rm] = v*(-xi2 + xij + xj2);

   k=2;
   km=n-2;
   while (k<nh) {
      rm -= nh;
      tr = r;
      r^=nh;
      for (m=(nh>>1); !((r^=m)&m); m>>=1) {
         ;
      }
      xi = f[r];
      xj = f[rm];
      xi2 = xi*xi;
      xj2 = xj*xj;
      xij = (2*xi*xj);
      f[r]  = v*(xi2 + xij - xj2);
      f[rm] = v*(-xi2 + xij + xj2);

      --km;
      ++k;
      rm += (tr-r);
      r += nh;
      xi = f[r];
      xj = f[rm];
      xi2 = xi*xi;
      xj2 = xj*xj;
      xij = (2*xi*xj);
      f[r]  = v*(xi2 + xij - xj2);
      f[rm] = v*(-xi2 + xij + xj2);
      --km;
      ++k;
   }
   return;
}
/* Public: FHT convolution */
int mp_fft(double *x, double *y, unsigned long length)
{
   unsigned long n;
   n = (length);
   if (n < 2) return MP_VAL;
   fht_dif_rec(x,(n));
   fht_dif_rec(y,(n));
   fht_conv_core(x, y,(n), 0.0);
   fht_dit_rec(y, (n));
   return MP_OKAY;
}
/* Public: FHT auto-convolution */
int mp_fft_sqr_d(double *x, unsigned long length)
{
   unsigned long n;
   n = (length);
   if (n < 2) return MP_VAL;
   fht_dif_rec(x,(n));
   fht_autoconv_core(x,(n), 0.0);
   fht_dit_rec(x, (n));
   return MP_OKAY;
}


#if (__STDC_VERSION__ >= 199901L) &&  (NEEDS_FE_RESET == 1)
/* Reset floating point environment settings  */
if (fesetenv(envp)) return MP_VAL;
#endif



#endif
