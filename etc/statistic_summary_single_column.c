#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <limits.h>

#define INPUT_BUFFER 256
#define REALOC_GROW 64

/*
   Takes a stream from stdin with small integers (one per line) and print some basic statistics
   that some might find useful. A bit like the 'summary()' command in R with the addition
   of the standard deviation and the first and third quantile.
   It has been written for a single use case with known input and not for general usage.
*/

/* Simplified sqrt within 1 ulp, no rounding; good enough to compute the SD */
/*
   From a copy of a drafted paper by Prof W. Kahan and K.C. Ng, written in May, 1986.

   That copy can be found in the file
   https://www.netlib.org/fdlibm/e_sqrt.c
*/
static double s_sqrt(double d)
{
   uint32_t high;

   uint32_t k, y0;
   double y;
   uint32_t T1[32] = {
      0,      1024,   3062,   5746,   9193,   13348,  18162,  23592,
      29598,  36145,  43202,  50740,  58733,  67158,  75992,  85215,
      83599,  71378,  60428,  50647,  41945,  34246,  27478,  21581,
      16499,  12183,  8588,   5674,   3403,   1742,   661,    130
   };

   union {
      double d;
      uint64_t u;
   } guts;

   guts.d = d;

   if (((guts.u >= 0x7FF0000000000001ULL) && (guts.u <= 0x7FF7FFFFFFFFFFFFULL))
       || ((guts.u >= 0xFFF0000000000001ULL) && (guts.u <= 0xFFF7FFFFFFFFFFFFULL))
       || ((guts.u >= 0x7FF8000000000001ULL) && (guts.u <= 0x7FFFFFFFFFFFFFFFULL))
       || ((guts.u >= 0xFFF8000000000001ULL) && (guts.u <= 0xFFFFFFFFFFFFFFFFULL))
       || (guts.u == 0x7FF0000000000000ULL)
       || (guts.u == 0x0000000000000000ULL)
       || (guts.u == 0x8000000000000000ULL)) {
      return d;
   }

   if (d < 0.0) {
      guts.u = 0xFFF8000000000000ULL;
      return guts.d;
   }


   high = guts.u >> 32;

   k = (high >> 1) + 0x1ff80000UL;
   y0 = k - T1[31 & (k >> 15)];

   guts.u = 0uLL;
   guts.u = y0;
   guts.u <<= 32;

   y = guts.d;

   y = (y + d/y) / 2.0;
   y = (y + d/y) / 2.0;
   y = y - (y - d/y) / 2.0;

   return y;
}

static double s_sd(long *array, int length, double mean)
{
   double sum = 0.0;
   int i;
   for (i = 0; i < length; i++) {
      sum += (array[i] - mean) * (array[i] - mean);
   }
   return s_sqrt(sum/(length-1));
}

static double s_sd_kurt(long *array, int length, double mean)
{
   double sum = 0.0;
   int i;
   for (i = 0; i < length; i++) {
      sum += (array[i] - mean) * (array[i] - mean);
   }
   return s_sqrt(sum/length);
}

static double s_kurtosis(long *array, int length, double mean){
   double sum = 0.0, quad = 0.0;
   int i;
   for (i = 0; i < length; i++) {
      quad = (array[i] - mean);
      quad = quad * quad;
      quad = quad * quad;
      sum += quad;
   }
   quad = s_sd_kurt(array, length, mean);
   quad = quad * quad;
   quad = quad * quad;
   return  ( (sum/(length)) / quad);
}

static double s_skewness(long *array, int length, double mean){
   double sum = 0.0, cube = 0.0;
   int i;
   for (i = 0; i < length; i++) {
      cube = (array[i] - mean);
      sum += cube * cube * cube;
   }
   cube = s_sd_kurt(array, length, mean);
   cube = cube * cube * cube;
   return  ( (sum/(length)) / cube);
}

/*
   Algorithm found in Section 2.3.4 "Finding the Median" in
   Wirth, Niklaus. "Algorithms + data structures = programs."
   No. 04; QA76. 6, W56.. 1976.
*/
static long k_smallest(long *array, int length, int k)
{
   int i, j, l, m;
   long tmp, element;

   l = 0;
   m = length - 1;
   while (l < m) {
      element = array[k];
      i = l;
      j = m;
      do {
         while (array[i] < element) {
            i++;
         }
         while (element < array[j]) {
            j--;
         }
         if (i <= j) {
            tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            i++;
            j--;
         }
      } while (i <= j);
      if (j < k) {
         l = i;
      }
      if (k < i) {
         m = j;
      }
   }
   return array[k];
}
static long s_median(long *array, int length)
{
   return k_smallest(array, length, ((length % 2) == 0) ? length/2 : (length / 2) - 1);
}
static double s_lower_quantile(long *array, int length)
{
   return k_smallest(array, length, ((length % 4) == 0) ? length/4 : (length / 4) - 1);
}
static double s_upper_quantile(long *array, int length)
{
   return  k_smallest(array, length, ((length % 4) == 0) ? (length/4) * 3: ((length / 4) - 1) * 3);
}
int main(int argc, char **argv)
{
   char str[INPUT_BUFFER] = "";
   char col_name[INPUT_BUFFER] = "";

   int base = 10;
   char *endptr;
   long val;
   int i = -1;

   int count = -1;
   long min = LONG_MAX;
   long max = LONG_MIN;
   long median = 0;
   double mean = 0.0;
   double sd = 0.0;
   double sk = 0.0;
   double ku = 0.0;
   double lq = 0.0;
   double uq = 0.0;

   long *array = NULL, *tmp;
   int alloc = -1;
   size_t size;

   int verbose = 0;

   if (argc != 1) {
      /* To avoid a warning with clang's -Weverything */
      if (argv[1][0] == 'v') {
         verbose = 1;
      }
   }
   /* Beware! Does not check anything! */
   while (fgets(str, INPUT_BUFFER, stdin) != NULL) {
      if (i < 0) {
         for (i = 0; i < INPUT_BUFFER; i++) {
            col_name[i] = str[i];
            if (col_name[i] == '\0')  {
               col_name[i - 1] = '\0';
               break;
            }
         }
         continue;
      }
      errno = 0;
      val = strtol(str, &endptr, base);
      if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
          || (errno != 0 && val == 0)) {
         fprintf(stderr,"conversion to number failed for input \"%s\".\n", str);
         exit(EXIT_FAILURE);
      }
      if (endptr == str) {
         fprintf(stderr, "Input \"%s\" does not contain any digits.\n", str);
         exit(EXIT_FAILURE);
      }
      count++;
      if (alloc < count) {
         size =  sizeof(long) * (size_t)((unsigned)count + (unsigned)REALOC_GROW);
         tmp = realloc(array, size);
         if (tmp == NULL) {
            fprintf(stderr, "Realloc failled to allocate %zu bytes\n", size);
            exit(EXIT_FAILURE);
         }
         array = tmp;
         alloc = alloc + REALOC_GROW;
      }
      array[count] = val;
      if (val < min) {
         min = val;
      }
      if (val > max) {
         max = val;
      }
      mean += (double)val;

   }
   count++;
   mean = mean/count;
   sd = s_sd(array, count, mean);
   median = s_median(array, count);
   if (count < 4) {
      fprintf(stderr, "Calculating the quantiles needs at least 4 entries not just %d\n", count);
   } else {
      lq = s_lower_quantile(array, count);
      uq = s_upper_quantile(array, count);
   }
   sk = s_skewness(array, count, mean);
   ku = s_kurtosis(array, count, mean);
   /* TODO: A bit wide, make two tables out of it? */
   if (verbose == 1) {
      printf("%8s%8s%8s%6s%7s%8s%10s%9s%8s%9s",
             "N", "min", "median", "mean", "max", "sd", "skew", "kurt", "25%", "75%\n");
   }
   printf("%5s %-4d %5ld %5ld %8.2f %5ld %8.2f %8.2f %8.2f %8.2f %8.2f\n",
          col_name, count, min, median, mean, max, sd, sk, ku, lq, uq);
   free(array);
   exit(EXIT_SUCCESS);
}


