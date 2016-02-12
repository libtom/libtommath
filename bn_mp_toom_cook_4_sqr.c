#include <tommath.h>
#ifdef BN_MP_TOOM_COOK_4_SQR_C
extern int TOOM_COOK_4_SQR_CO;

#ifdef USE_THREADS_FOR_TOOM_5_NOT
#ifndef LMP_NUM_THREADS
#define LMP_NUM_THREADS 4
#endif

#include <pthread.h>

struct toom_sqr_4_param {
    mp_int *a;
    mp_int *c;
    int depth;
    int err;
};

static void  *mp_toom_cook_4_sqr_threaded(void *p)
{
   mp_int w1, w2, w3, w4, w5, w6, w7;
   mp_int t1, t2, t3, t4, t5, t6, t7, t8;
   mp_int tmp1;
   mp_int a0, a1, a2, a3;

   int e = MP_OKAY;
   int B, count;
   struct toom_sqr_4_param *params = p;



   B = (params->a->used) / 4;

   if (params->a->used < TOOM_COOK_4_SQR_CO) {
      params->err = mp_sqr(params->a,params->c);
      return NULL;
   }

   if ((e =
           mp_init_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7, &tmp1, &t1, &t2, &t3,
                         &t4, &t5, &t6, &t7, &t8, NULL)) != MP_OKAY) {
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
   if ((e = mp_init_size(&a3, B)) != MP_OKAY) {
      goto ERRa3;
   }

   // pgp  a = a3*B^3 + a2*B^2 + a1*B + a0;
   for (count = 0; count < params->a->used; count++) {
      switch (count / B) {
      case 0:
         a0.dp[count] = params->a->dp[count];
         a0.used++;
         break;
      case 1:
         a1.dp[count - B] = params->a->dp[count];
         a1.used++;
         break;
      case 2:
         a2.dp[count - 2 * B] = params->a->dp[count];
         a2.used++;
         break;
      case 3:
         a3.dp[count - 3 * B] = params->a->dp[count];
         a3.used++;
         break;
      default:
         a3.dp[count - 3 * B] = params->a->dp[count];
         a3.used++;
         break;
      }
   }
   mp_clamp(&a0);
   mp_clamp(&a1);
   mp_clamp(&a2);
   mp_clamp(&a3);


   struct toom_sqr_4_param tparam_one = { .a = &a0, .c = &w1, .depth = params->depth + 1 };

   if(params->depth < LMP_NUM_THREADS){
      pthread_t thread_one;
// w1 = a0^2
      pthread_create(&thread_one, NULL, mp_toom_cook_4_sqr_threaded, &tparam_one);
      pthread_join(thread_one, NULL);
   } else {
      mp_toom_cook_4_sqr_threaded(&tparam_one);
   }
//TODO: real error handling!
   if (params->err != MP_OKAY)
      goto ERR;


// w2 = 2 *a0 *a1
   if ((e = mp_mul(&a0, &a1, &w2)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_mul_2d(&w2, 1, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w3 = (a0 + a1 - a2 - a3) * (a0 - a1 - a2 + a3)

   if ((e = mp_sub(&a0,&a2,&t1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&a1,&a3,&t2)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&t1,&t2,&t3)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&t1,&t2,&t4)) != MP_OKAY) {
      goto ERR;;
   }

   /*
     if ((e = mp_add(&a0, &a1, &t1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&t1, &a2, &t1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&t1, &a3, &t1)) != MP_OKAY) {
       goto ERR;
     }

     if ((e = mp_sub(&a0, &a1, &t2)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&t2, &a2, &t2)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_add(&t2, &a3, &t2)) != MP_OKAY) {
       goto ERR;
     }
   */
   if ((e = mp_mul(&t3, &t4, &w3)) != MP_OKAY) {
      goto ERR;
   }

// w4 = (a0 + a1 + a2 + a3 )^2
   if ((e = mp_add(&a0, &a1, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w4, &a2, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w4, &a3, &w4)) != MP_OKAY) {
      goto ERR;
   }


   struct toom_sqr_4_param tparam_two = { .a = &w4, .c = &w4, .depth = params->depth + 1 };

   if(params->depth < LMP_NUM_THREADS){
      pthread_t thread_one;
// w4 = w4^2
      pthread_create(&thread_one, NULL, mp_toom_cook_4_sqr_threaded, &tparam_one);
      pthread_join(thread_one, NULL);
   } else {
      mp_toom_cook_4_sqr_threaded(&tparam_two);
   }
//TODO: real error handling!
   if (params->err != MP_OKAY)
      goto ERR;

// w5  = 2*(a0 - a2)*(a1 - a3)
   /*
     if ((e = mp_sub(&a0, &a2, &t1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&a1, &a3, &t2)) != MP_OKAY) {
       goto ERR;
     }
   */

   if ((e = mp_mul(&t1, &t2, &w5)) != MP_OKAY) {
      goto ERR;
   }


   if ((e = mp_mul_2d(&w5, 1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w6 = 2*a3*a2
   if ((e = mp_mul(&a3, &a2, &w6)) != MP_OKAY) {
      goto ERR;
   }


   if ((e = mp_mul_2d(&w6, 1, &w6)) != MP_OKAY) {
      goto ERR;
   }


   struct toom_sqr_4_param tparam_three = { .a = &a3, .c = &w7, .depth = params->depth + 1 };

   if(params->depth < LMP_NUM_THREADS){
      pthread_t thread_one;
// w7 = a3^2
      pthread_create(&thread_one, NULL, mp_toom_cook_4_sqr_threaded, &tparam_one);
      pthread_join(thread_one, NULL);
   } else {
      mp_toom_cook_4_sqr_threaded(&tparam_three);
   }
//TODO: real error handling!
   if (params->err != MP_OKAY)
      goto ERR;



// T1 = w3 + w4
   if ((e = mp_add(&w3, &w4, &t1)) != MP_OKAY) {
      goto ERR;
   }
// T2 = (T1 + w5)/2
   if ((e = mp_add(&t1, &w5, &t2)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_div_2d(&t2, 1, &t2, NULL)) != MP_OKAY) {
      goto ERR;
   }
// T3 = w2 + w6
   if ((e = mp_add(&w2, &w6, &t3)) != MP_OKAY) {
      goto ERR;
   }
// T4 = T2 - T3
   if ((e = mp_sub(&t2, &t3, &t4)) != MP_OKAY) {
      goto ERR;
   }
// T5 = T3 - w5
   if ((e = mp_sub(&t3, &w5, &t5)) != MP_OKAY) {
      goto ERR;
   }
// T6 = T4 - w3
   if ((e = mp_sub(&t4, &w3, &t6)) != MP_OKAY) {
      goto ERR;
   }
// T7 = T4 - w1
   if ((e = mp_sub(&t4, &w1, &t7)) != MP_OKAY) {
      goto ERR;
   }
// T8 = T6 - w7
   if ((e = mp_sub(&t6, &w7, &t8)) != MP_OKAY) {
      goto ERR;
   }
// P = w7 *x^6 + w6 *x^5 + T7 *x^4 + T5 *x^3 + T8 *x^2 + w2 *x + w1
   if ((e = mp_copy(&w1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w2, B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&t8, 2 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &t8, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&t5, 3 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &t5, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&t7, 4 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &t7, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&w6, 5 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w6, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&w7, 6 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w7, params->c)) != MP_OKAY) {
      goto ERR;
   }

ERR:
ERRa3:
   mp_clear(&a3);
ERRa2:
   mp_clear(&a2);
ERRa1:
   mp_clear(&a1);
ERRa0:
   mp_clear(&a0);
ERR0:
   mp_clear_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7,
                  &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &tmp1, NULL);
   params->err = e;
   return NULL;
}


int mp_toom_cook_4_sqr(mp_int *a, mp_int *c){

    struct toom_sqr_4_param tparam = { .a = a, .c = c , .depth = 0};
    mp_toom_cook_4_sqr_threaded(&tparam);
    mp_exch(tparam.c,c);
    return tparam.err;
}

#else
int mp_toom_cook_4_sqr(mp_int *a, mp_int *c)
{
   mp_int w1, w2, w3, w4, w5, w6, w7;
   mp_int t1, t2, t3, t4, t5, t6, t7, t8;
   mp_int tmp1;
   mp_int a0, a1, a2, a3;

#ifdef USE_OPEN_MP_NOT
   int e1,e2;
#endif

   int e = MP_OKAY;
   int B, count;

   B = (a->used) / 4;

   if (a->used < TOOM_COOK_4_SQR_CO) {
      if ((e = mp_sqr(a, c)) != MP_OKAY) {
         return e;
      }
      return MP_OKAY;
   }

   if ((e =
           mp_init_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7, &tmp1, &t1, &t2, &t3,
                         &t4, &t5, &t6, &t7, &t8, NULL)) != MP_OKAY) {
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
   if ((e = mp_init_size(&a3, B)) != MP_OKAY) {
      goto ERRa3;
   }

   // pgp  a = a3*B^3 + a2*B^2 + a1*B + a0;
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
      case 3:
         a3.dp[count - 3 * B] = a->dp[count];
         a3.used++;
         break;
      default:
         a3.dp[count - 3 * B] = a->dp[count];
         a3.used++;
         break;
      }
   }
   mp_clamp(&a0);
   mp_clamp(&a1);
   mp_clamp(&a2);
   mp_clamp(&a3);


#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel
   #pragma omp single nowait
   {
      #pragma omp task shared(e1)
      e1 = mp_sqr(&a0, &w1);
      #pragma omp task shared(e2)
      e2 =mp_mul(&a0, &a1, &w2);
   }
   if ((e1 != MP_OKAY) || (e2 != MP_OKAY))
      goto ERR;
#else

// w1 = a0^2
   if ((e = mp_sqr(&a0, &w1)) != MP_OKAY) {
      goto ERR;
   }

// w2 = 2 *a0 *a1
   if ((e = mp_mul(&a0, &a1, &w2)) != MP_OKAY) {
      goto ERR;
   }
#endif
   if ((e = mp_mul_2d(&w2, 1, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w3 = (a0 + a1 - a2 - a3) * (a0 - a1 - a2 + a3)

   if ((e = mp_sub(&a0,&a2,&t1)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_sub(&a1,&a3,&t2)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_add(&t1,&t2,&t3)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_sub(&t1,&t2,&t4)) != MP_OKAY) {
      return e;
   }

   /*
     if ((e = mp_add(&a0, &a1, &t1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&t1, &a2, &t1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&t1, &a3, &t1)) != MP_OKAY) {
       goto ERR;
     }

     if ((e = mp_sub(&a0, &a1, &t2)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&t2, &a2, &t2)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_add(&t2, &a3, &t2)) != MP_OKAY) {
       goto ERR;
     }
   */
#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel

   {
      #pragma omp critical
      e1 = mp_mul(&t3, &t4, &w3);
   }
   if (e1 != MP_OKAY)
      goto ERR;
#else


   if ((e = mp_mul(&t3, &t4, &w3)) != MP_OKAY) {
      goto ERR;
   }
#endif
// w4 = (a0 + a1 + a2 + a3 )^2
   if ((e = mp_add(&a0, &a1, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w4, &a2, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w4, &a3, &w4)) != MP_OKAY) {
      goto ERR;
   }

#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel

   {
      #pragma omp critical
      e1 = mp_sqr(&w4, &w4);
   }
   if (e1 != MP_OKAY)
      goto ERR;
#else

   if ((e = mp_sqr(&w4, &w4)) != MP_OKAY) {
      goto ERR;
   }
#endif

// w5  = 2*(a0 - a2)*(a1 - a3)
   /*
     if ((e = mp_sub(&a0, &a2, &t1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&a1, &a3, &t2)) != MP_OKAY) {
       goto ERR;
     }
   */

#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel

   {
      #pragma omp critical
      e1 = mp_mul(&t1, &t2, &w5);
   }
   if (e1 != MP_OKAY)
      goto ERR;
#else
   if ((e = mp_mul(&t1, &t2, &w5)) != MP_OKAY) {
      goto ERR;
   }
#endif

   if ((e = mp_mul_2d(&w5, 1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w6 = 2*a3*a2
#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel

   {
      #pragma omp critical
      e1 = mp_mul(&a3, &a2, &w6);
   }
   if (e1 != MP_OKAY)
      goto ERR;
#else
   if ((e = mp_mul(&a3, &a2, &w6)) != MP_OKAY) {
      goto ERR;
   }
#endif

   if ((e = mp_mul_2d(&w6, 1, &w6)) != MP_OKAY) {
      goto ERR;
   }

#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel

   {
      #pragma omp critical
      e1 = mp_sqr(&a3, &w7);
   }
   if (e1 != MP_OKAY)
      goto ERR;
#else
// w7 = a3^2
   if ((e = mp_sqr(&a3, &w7)) != MP_OKAY) {
      goto ERR;
   }
#endif

// T1 = w3 + w4
   if ((e = mp_add(&w3, &w4, &t1)) != MP_OKAY) {
      goto ERR;
   }
// T2 = (T1 + w5)/2
   if ((e = mp_add(&t1, &w5, &t2)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_div_2d(&t2, 1, &t2, NULL)) != MP_OKAY) {
      goto ERR;
   }
// T3 = w2 + w6
   if ((e = mp_add(&w2, &w6, &t3)) != MP_OKAY) {
      goto ERR;
   }
// T4 = T2 - T3
   if ((e = mp_sub(&t2, &t3, &t4)) != MP_OKAY) {
      goto ERR;
   }
// T5 = T3 - w5
   if ((e = mp_sub(&t3, &w5, &t5)) != MP_OKAY) {
      goto ERR;
   }
// T6 = T4 - w3
   if ((e = mp_sub(&t4, &w3, &t6)) != MP_OKAY) {
      goto ERR;
   }
// T7 = T4 - w1
   if ((e = mp_sub(&t4, &w1, &t7)) != MP_OKAY) {
      goto ERR;
   }
// T8 = T6 - w7
   if ((e = mp_sub(&t6, &w7, &t8)) != MP_OKAY) {
      goto ERR;
   }
// P = w7 *x^6 + w6 *x^5 + T7 *x^4 + T5 *x^3 + T8 *x^2 + w2 *x + w1
   if ((e = mp_copy(&w1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w2, B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&t8, 2 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &t8, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&t5, 3 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &t5, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&t7, 4 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &t7, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&w6, 5 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w6, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_lshd(&w7, 6 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w7, c)) != MP_OKAY) {
      goto ERR;
   }

ERR:
ERRa3:
   mp_clear(&a3);
ERRa2:
   mp_clear(&a2);
ERRa1:
   mp_clear(&a1);
ERRa0:
   mp_clear(&a0);
ERR0:
   mp_clear_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7,
                  &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &tmp1, NULL);
   return e;
}
#endif
#endif
