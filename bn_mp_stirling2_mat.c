#include <tommath.h>
#ifdef BN_MP_STIRLING2_MAT_C
static mp_int **stirling2_mat = NULL;
static unsigned long stirling2_n = 0;
static unsigned long stirling2_m = 0;


int mp_stirling2_mat(unsigned long n, unsigned long m, mp_int *c)
{
   unsigned long i, j, k;
   mp_int temp;
   int e;

   /* We assume the caller knows what it does and return only in case of error */
   if (n < m) {
      return MP_VAL;
   }
   /* Not necessarily an error */
   if (m == 0 || n == 0) {
      return MP_VAL;
   }

   /* Check if already cached */
   if (stirling2_n >= n && stirling2_m >= m) {
      if ((e = mp_copy(&(stirling2_mat)[n][m], c)) != MP_OKAY) {
         return e;
      }
      return MP_OKAY;
   }

   /* TODO: check if array already exists and reallocate but for */
   /*       now, we just free the memory and start from scratch */
   if (stirling2_mat != NULL) {
      for (k = 0; k <= stirling2_n; k++) {
         for (i = 0; i <= stirling2_m; i++) {
            mp_clear(&stirling2_mat[k][i]);
         }
         free(stirling2_mat[k]);
      }
      free(stirling2_mat);
      stirling2_mat = NULL;
      stirling2_n = 0;
      stirling2_m = 0;
   }
   /* Allocate memory for n rows with m columns */
   /* Be aware that it will allocate a lot of memory */
   stirling2_mat = malloc(sizeof(mp_int *) * (n + 1));
   if (stirling2_mat == NULL) {
      return MP_MEM;
   }
   for (k = 0; k <= n; k++) {
      stirling2_mat[k] = malloc(sizeof(mp_int) * (m + 1));
      if (stirling2_mat[k] == NULL) {
         return MP_MEM;
      }
      for (i = 0; i <= m; i++) {
         mp_init(&stirling2_mat[k][i]);
      }
   }
   if ((e = mp_init(&temp)) != MP_OKAY) {
      return e;
   }
   mp_set(&(stirling2_mat)[0][0], 1);

   for (i = 1; i <= n; i++) {
      for (j = 1; j <= m; j++) {
         if (j <= i) {
            // S(i,j) = S(i - 1, j - 1) +      j  * S(i - 1, j)
            if ((e = mp_set_int(&temp, j)) != MP_OKAY) {
               return e;
            }
            if ((e = mp_mul(&(stirling2_mat)[i - 1][j], &temp, &temp)) != MP_OKAY) {
               return e;
            }
            if ((e =
                    mp_add(&(stirling2_mat)[i - 1][j - 1], &temp,
                           &(stirling2_mat)[i][j])) != MP_OKAY) {
               return e;
            }
         } else {
            mp_set(&(stirling2_mat)[i][j], 0);
         }
      }
   }

   stirling2_n = n;
   stirling2_m = m;

   if ((e = mp_copy(&(stirling2_mat)[n][m], c)) != MP_OKAY) {
      return e;
   }

   return MP_OKAY;
}

void mp_stirling2_free(void)
{
   unsigned long i, k;
   if (stirling2_mat != NULL) {
      for (k = 0; k <= stirling2_n; k++) {
         for (i = 0; i <= stirling2_m; i++) {
            mp_clear(&stirling2_mat[k][i]);
         }
         free(stirling2_mat[k]);
      }
      free(stirling2_mat);
      stirling2_mat = NULL;
   }
   stirling2_n = 0;
   stirling2_m = 0;
}
#endif
