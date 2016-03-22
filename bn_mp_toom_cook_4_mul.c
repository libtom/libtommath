#include <tommath.h>
#ifdef BN_MP_TOOM_COOK_4_MUL_C
extern int TOOM_COOK_4_MUL_CO;

#ifdef USE_THREADS_FOR_TOOM_5_NOT
#ifndef LMP_NUM_THREADS
#define LMP_NUM_THREADS 4
#endif

#include <pthread.h>

struct toom_mul_4_param {
    mp_int *a;
    mp_int *b;
    mp_int *c;
    int depth;
    int err;
};

static void *mp_toom_cook_4_mul_threaded(void *p)
{
   mp_int w1, w2, w3, w4, w5, w6, w7;
   mp_int tmp1, tmp2;
   mp_int a0, a1, a2, a3;
   mp_int b0, b1, b2, b3;
   int e = MP_OKAY;
   int B, count, sign;

   struct toom_mul_4_param *params = p;

   B = ((MAX(params->a->used, params->b->used)) / 4);

   // may come from FFT and did not get sign handled
   sign = (params->a->sign != params->b->sign) ? MP_NEG : MP_ZPOS;

   if (MIN(params->a->used, params->b->used) < TOOM_COOK_4_MUL_CO) {
      params->err = mp_mul(params->a,params->b,params->c);
      params->c->sign = sign;
      return NULL;
   }


   if ((e = mp_init_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7, &tmp1, &tmp2,
                          &a0, &a1, &a2, &a3,&b0, &b1,&b2,&b3,
                          NULL)) != MP_OKAY) {
      goto ERR0;
      //goto ERR;
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

   if ((e = mp_init_size(&b0, B)) != MP_OKAY) {
      goto ERRb0;
   }
   if ((e = mp_init_size(&b1, B)) != MP_OKAY) {
      goto ERRb1;
   }
   if ((e = mp_init_size(&b2, B)) != MP_OKAY) {
      goto ERRb2;
   }
   if ((e = mp_init_size(&b3, B)) != MP_OKAY) {
      goto ERRb3;
   }
   // a = a3*B^3 + a2*B^2 + a1*B + a0;
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
   // b = b3*B^3 + b2*B^2 + b1*B + b0;
   for (count = 0; count < params->b->used; count++) {
      switch (count / B) {
      case 0:
         b0.dp[count] = params->b->dp[count];
         b0.used++;
         break;
      case 1:
         b1.dp[count - B] = params->b->dp[count];
         b1.used++;
         break;
      case 2:
         b2.dp[count - 2 * B] = params->b->dp[count];
         b2.used++;
         break;
      case 3:
         b3.dp[count - 3 * B] = params->b->dp[count];
         b3.used++;
         break;
      default:
         b3.dp[count - 3 * B] = params->b->dp[count];
         b3.used++;
         break;
      }
   }
   mp_clamp(&b0);
   mp_clamp(&b1);
   mp_clamp(&b2);
   mp_clamp(&b3);

   struct toom_mul_4_param tparam_one = { .a = &a3, .b = &b3, .c = &w1, .depth = params->depth + 1 };
   if(params->depth < LMP_NUM_THREADS){
      pthread_t thread_one;
// w1 = a3*b3
      pthread_create(&thread_one, NULL, mp_toom_cook_4_mul_threaded, &tparam_one);

      pthread_join(thread_one, NULL);

   } else {
      mp_toom_cook_4_mul_threaded(&tparam_one);
   }

// w2 = (8*a3 + 4*a2 + 2*a1 + a0)*(8*b3 + 4*b2 + 2*b1 + b0)
   if ((e = mp_mul_2d(&a3, 3, &w2)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a2, 2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a1, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w2, &a0, &w2)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_mul_2d(&b3, 3, &w3)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b2, 2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w3, &tmp1, &w3)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b1, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w3, &tmp1, &w3)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w3, &b0, &w3)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_mul(&w2, &w3, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w3 = (+a3 + a2 + a1 + a0)*(+b3 + b2 + b1 + b0)
   if ((e = mp_add(&a3, &a2, &w3)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&a1, &a0, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w3, &tmp1, &w3)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_add(&b3, &b2, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&b1, &b0, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w4, &tmp1, &w4)) != MP_OKAY) {
      goto ERR;
   }

   struct toom_mul_4_param tparam_two = { .a = &w3, .b = &w4, .c = &w3, .depth = params->depth + 1 };
   if(params->depth < LMP_NUM_THREADS){
      pthread_t thread_one;
// w3 = w3*w4
      pthread_create(&thread_one, NULL, mp_toom_cook_4_mul_threaded, &tparam_two);

      pthread_join(thread_one, NULL);

   } else {
      mp_toom_cook_4_mul_threaded(&tparam_two);
   }



// w4 = (-a3+a2-a1+a0)*(-b3+b2-b1+b0)
//    = (+a2-a1 + a0-a3)*(+b2-b1 + b0-b3)

   if ((e = mp_sub(&a2, &a1, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&a0, &a3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w4, &tmp1, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&b2, &b1, &w5)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&b0, &b3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }


   struct toom_mul_4_param tparam_three = {.a = &w4,.b = &w5,.c = &w4, .depth = params->depth + 1};
   if(params->depth < LMP_NUM_THREADS){
      pthread_t thread_one;
// w4 = w4*w5
      pthread_create(&thread_one, NULL, mp_toom_cook_4_mul_threaded, &tparam_three);
      pthread_join(thread_one, NULL);
   } else {
      mp_toom_cook_4_mul_threaded(&tparam_three);
   }


// w5 = (+8*a0+4*a1+2*a2+a3)*(+8*b0+4*b1+2*b2+b3)
   if ((e = mp_mul_2d(&a0, 3, &w5)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a1, 2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a2, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w5, &a3, &w5)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_mul_2d(&b0, 3, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b1, 2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b2, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &b3, &w6)) != MP_OKAY) {
      goto ERR;
   }
   struct toom_mul_4_param tparam_four = {.a = &w5,.b = &w6,.c = &w5, .depth = params->depth + 1};
   if(params->depth < LMP_NUM_THREADS){
      pthread_t thread_one;
// w5 = w6*w5
      pthread_create(&thread_one, NULL, mp_toom_cook_4_mul_threaded, &tparam_four);
      pthread_join(thread_one, NULL);
   } else {
      mp_toom_cook_4_mul_threaded(&tparam_four);
   }

// w6 = (-8*a0+4*a1-2*a2+a3)*(-8*b0+4*b1-2*b2+b3)
//    = (+4*a1-2*a2+a3-8*a0)*(+4*b1-2*b2+b3-8*b0)
   if ((e = mp_mul_2d(&a1, 2, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a2, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &a3, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a0, 3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_mul_2d(&b1, 2, &w7)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b2, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w7, &tmp1, &w7)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w7, &b3, &w7)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b0, 3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w7, &tmp1, &w7)) != MP_OKAY) {
      goto ERR;
   }

   struct toom_mul_4_param tparam_five = {.a = &w6,.b = &w7,.c = &w6, .depth = params->depth + 1};
   struct toom_mul_4_param tparam_six = {.a = &a0,.b = &b0,.c = &w7, .depth = params->depth + 1};
   if(params->depth < LMP_NUM_THREADS){
      pthread_t thread_one, thread_two;
// w6 = w7*w6
      pthread_create(&thread_one, NULL, mp_toom_cook_4_mul_threaded, &tparam_five);
// w7 = a0*b0
      pthread_create(&thread_two, NULL, mp_toom_cook_4_mul_threaded, &tparam_six);
      pthread_join(thread_one, NULL);
      pthread_join(thread_two, NULL);
   } else {
      mp_toom_cook_4_mul_threaded(&tparam_five);
      mp_toom_cook_4_mul_threaded(&tparam_six);
   }


// w2 = w2 + w5;
   if ((e = mp_add(&w2, &w5, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w4 = w4 - w3;
   if ((e = mp_sub(&w4, &w3, &w4)) != MP_OKAY) {
      goto ERR;
   }
// w6 = w6 - w5;
   if ((e = mp_sub(&w6, &w5, &w6)) != MP_OKAY) {
      goto ERR;
   }
// w4 = w4 /2 ;
   if ((e = mp_div_2d(&w4, 1, &w4, NULL)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 - w1;
   if ((e = mp_sub(&w5, &w1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 - (64*w7) ;
   if ((e = mp_mul_2d(&w7, 6, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w3 = w3 + w4;
   if ((e = mp_add(&w3, &w4, &w3)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 *2 ;
   if ((e = mp_mul_2d(&w5, 1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 + w6;
   if ((e = mp_add(&w5, &w6, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 - (65*w3) ;
   if ((e = mp_mul_d(&w3, 65, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w3 = w3 - w1;
   if ((e = mp_sub(&w3, &w1, &w3)) != MP_OKAY) {
      goto ERR;
   }
// w3 = w3 - w7;
   if ((e = mp_sub(&w3, &w7, &w3)) != MP_OKAY) {
      goto ERR;
   }
// w4 = -w4;
   if ((e = mp_neg(&w4, &w4)) != MP_OKAY) {
      goto ERR;
   }
// w6 = -w6;
   if ((e = mp_neg(&w6, &w6)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 + (45*w3) ;
   if ((e = mp_mul_d(&w3, 45, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 - (8*w3) ;
   if ((e = mp_mul_2d(&w3, 3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5/24;
   if ((e = mp_div_d(&w5, 24, &w5, NULL)) != MP_OKAY) {
      goto ERR;
   }

// w6 = w6 - w2;
   if ((e = mp_sub(&w6, &w2, &w6)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 - (16*w4) ;
   if ((e = mp_mul_2d(&w4, 4, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2/18;
   if ((e = mp_div_d(&w2, 18, &w2, NULL)) != MP_OKAY) {
      goto ERR;
   }
// w3 = w3 - w5;
   if ((e = mp_sub(&w3, &w5, &w3)) != MP_OKAY) {
      goto ERR;
   }
// w4 = w4 - w2;
   if ((e = mp_sub(&w4, &w2, &w4)) != MP_OKAY) {
      goto ERR;
   }
// w6 = w6 + (30*w2) ;
   if ((e = mp_mul_d(&w2, 30, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }
// w6 = w6/60;
   if ((e = mp_div_d(&w6, 60, &w6, NULL)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 - w6;
   if ((e = mp_sub(&w2, &w6, &w2)) != MP_OKAY) {
      goto ERR;
   }



#   if 0

   /*
     if ((e = mp_mul_2d(&w3, 6, &tmp1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_add(&w3, &tmp1, &tmp1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&w2, &tmp1, &w2)) != MP_OKAY) {
       goto ERR;
     }
   */
   mp_mul_d(&w3, 65, &tmp1);
   mp_sub(&w2, &tmp1, &w2);

// w5 = w5/24;
// all divisions are exact, so we can play with the prime factors
// or write a function to do exact division, which should do it faster
// w5 /= 24      = (w5 / 8) / 3 = (w5 >> 3) / 3
   /*
     if ((e = mp_div_2d(&w5, 3, &w5, NULL)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_div_3(&w5, &w5, NULL)) != MP_OKAY) {
       goto ERR;
     }
   */
   mp_div_d(&w5, 24, &w5, NULL);

// w2 = w2/18;
// w2 /= 18      = ((w2 >> 1) / 3 ) / 3
   /*
     if ((e = mp_div_2d(&w2, 1, &w2, NULL)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_div_3(&w2, &w2, NULL)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_div_3(&w2, &w2, NULL)) != MP_OKAY) {
       goto ERR;
     }
   */
   mp_div_d(&w2, 18, &w2, NULL);

// w6 += w2 * 30;
// w6 += (30*w2) = (w2 << 5) - (w2 << 1)
   /*
     if ((e = mp_mul_2d(&w2, 1, &tmp2)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_mul_2d(&tmp2, 4, &tmp1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&tmp1, &tmp2, &tmp1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_add(&w6, &tmp1, &w6)) != MP_OKAY) {
       goto ERR;
     }
   */
   mp_mul_d(&w2, 30, &tmp1);
   mp_add(&w6, &tmp1, &w6);
// w6 = w6/60;
// w6 /= 60      = ((w6 >> 2) / 3 ) / 5
   if ((e = mp_div_d(&w6, 60, &w6, NULL)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 - w6;
   if ((e = mp_sub(&w2, &w6, &w2)) != MP_OKAY) {
      goto ERR;
   }
#   endif
// S = w7 + w6*B + w5*B^2 + w4*B^3 + w3*B^4 + w2*B^5 + w1*B^6;

   if ((e = mp_copy(&w7, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w6, B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w6, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w5, 2 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w5, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w4, 3 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w4, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w3, 4 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w2, 5 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w1, 6 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w1, params->c)) != MP_OKAY) {
      goto ERR;
   }

   params->c->sign = sign;

// print(S - a*b)
ERR:

ERRb3:
   mp_clear(&b3);
ERRb2:
   mp_clear(&b2);
ERRb1:
   mp_clear(&b1);
ERRb0:
   mp_clear(&b0);
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
//     &a0, &a1, &a2, &a3,&b0, &b1,&b2,&b3,
                  &tmp1, &tmp2, NULL);
   params->err = e;
   return NULL;
}
int mp_toom_cook_4_mul(mp_int *a, mp_int *b, mp_int *c){
    struct toom_mul_4_param tparam = { .a = a, .b = b, .c = c , .depth = 0};
    mp_toom_cook_4_mul_threaded(&tparam);
    mp_exch(tparam.c,c);
    return tparam.err;
}

#else
int mp_toom_cook_4_mul(mp_int *a, mp_int *b, mp_int *c)
{
   mp_int w1, w2, w3, w4, w5, w6, w7;
   mp_int tmp1, tmp2;
   mp_int a0, a1, a2, a3;
   mp_int b0, b1, b2, b3;

#ifdef USE_OPEN_MP_NOT
   int e1,e2;
#endif

   int e = MP_OKAY;
   int B, count, sign;

   B = ((MAX(a->used, b->used)) / 4);

   // may come from FFT and did not get sign handled
   sign = (a->sign != b->sign) ? MP_NEG : MP_ZPOS;

   if (MIN(a->used, b->used) < TOOM_COOK_4_MUL_CO) {
      if ((e = mp_mul(a, b, c)) != MP_OKAY) {
         return e;
      }
      c->sign = sign;
      return MP_OKAY;
   }


   if ((e = mp_init_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7, &tmp1, &tmp2,
                          //&a0, &a1, &a2, &a3,&b0, &b1,&b2,&b3,
                          NULL)) != MP_OKAY) {
      goto ERR0;
      //goto ERR;
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

   if ((e = mp_init_size(&b0, B)) != MP_OKAY) {
      goto ERRb0;
   }
   if ((e = mp_init_size(&b1, B)) != MP_OKAY) {
      goto ERRb1;
   }
   if ((e = mp_init_size(&b2, B)) != MP_OKAY) {
      goto ERRb2;
   }
   if ((e = mp_init_size(&b3, B)) != MP_OKAY) {
      goto ERRb3;
   }
   // a = a3*B^3 + a2*B^2 + a1*B + a0;
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
   // b = b3*B^3 + b2*B^2 + b1*B + b0;
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
      case 3:
         b3.dp[count - 3 * B] = b->dp[count];
         b3.used++;
         break;
      default:
         b3.dp[count - 3 * B] = b->dp[count];
         b3.used++;
         break;
      }
   }
   mp_clamp(&b0);
   mp_clamp(&b1);
   mp_clamp(&b2);
   mp_clamp(&b3);
   /*
       if ((e = mp_mod_2d(a, DIGIT_BIT * B, &a0)) != MP_OKAY) {
          goto ERR;
       }
       if ((e = mp_copy(a, &a1)) != MP_OKAY) {
          goto ERR;
       }
       mp_rshd(&a1, B);
       mp_mod_2d(&a1, DIGIT_BIT * B, &a1);

       if ((e = mp_copy(a, &a2)) != MP_OKAY) {
          goto ERR;
       }
       mp_rshd(&a2, B*2);
       mp_mod_2d(&a2, DIGIT_BIT * B, &a2);

       if ((e = mp_copy(a, &a3)) != MP_OKAY) {
          goto ERR;
       }
       mp_rshd(&a3, B*3);
       //mp_mod_2d(&a3, DIGIT_BIT * B, &a3);


       if ((e = mp_mod_2d(b, DIGIT_BIT * B, &b0)) != MP_OKAY) {
          goto ERR;
       }
       if ((e = mp_copy(b, &b1)) != MP_OKAY) {
          goto ERR;
       }
       mp_rshd(&b1, B);
       mp_mod_2d(&b1, DIGIT_BIT * B, &b1);

       if ((e = mp_copy(b, &b2)) != MP_OKAY) {
          goto ERR;
       }
       mp_rshd(&b2, B*2);
       mp_mod_2d(&b2, DIGIT_BIT * B, &b2);

       if ((e = mp_copy(b, &b3)) != MP_OKAY) {
          goto ERR;
       }
       mp_rshd(&b3, B*3);
       //mp_mod_2d(&b3, DIGIT_BIT * B, &b3);
   */
// w1 = a3*b3

#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel

   {
      #pragma omp critical
      e1 = mp_mul(&a3, &b3, &w1);
   }
   if (e1 != MP_OKAY)
      goto ERR;
#else
   if ((e = mp_mul(&a3, &b3, &w1)) != MP_OKAY) {                 // <--
      goto ERR;
   }
#endif
// w2 = (8*a3 + 4*a2 + 2*a1 + a0)*(8*b3 + 4*b2 + 2*b1 + b0)
   if ((e = mp_mul_2d(&a3, 3, &w2)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a2, 2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a1, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w2, &a0, &w2)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_mul_2d(&b3, 3, &w3)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b2, 2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w3, &tmp1, &w3)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b1, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w3, &tmp1, &w3)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w3, &b0, &w3)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_mul(&w2, &w3, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w3 = (+a3 + a2 + a1 + a0)*(+b3 + b2 + b1 + b0)
   if ((e = mp_add(&a3, &a2, &w3)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&a1, &a0, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w3, &tmp1, &w3)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_add(&b3, &b2, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&b1, &b0, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w4, &tmp1, &w4)) != MP_OKAY) {
      goto ERR;
   }

#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel

   {
      #pragma omp critical
      e1 = mp_mul(&w3, &w4, &w3);
   }
   if (e1 != MP_OKAY)
      goto ERR;
#else

   if ((e = mp_mul(&w3, &w4, &w3)) != MP_OKAY) {                 // <--
      goto ERR;
   }

#endif

// w4 = (-a3+a2-a1+a0)*(-b3+b2-b1+b0)
//    = (+a2-a1 + a0-a3)*(+b2-b1 + b0-b3)

   if ((e = mp_sub(&a2, &a1, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&a0, &a3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w4, &tmp1, &w4)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&b2, &b1, &w5)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&b0, &b3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul(&w4, &w5, &w4)) != MP_OKAY) {                 // <--
      goto ERR;
   }

// w5 = (+8*a0+4*a1+2*a2+a3)*(+8*b0+4*b1+2*b2+b3)
   if ((e = mp_mul_2d(&a0, 3, &w5)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a1, 2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a2, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w5, &a3, &w5)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_mul_2d(&b0, 3, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b1, 2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b2, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &b3, &w6)) != MP_OKAY) {
      goto ERR;
   }

#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel

   {
      #pragma omp critical
      e1 = mp_mul(&w5, &w6, &w5);
   }
   if (e1 != MP_OKAY)
      goto ERR;
#else

   if ((e = mp_mul(&w5, &w6, &w5)) != MP_OKAY) {                 // <--
      goto ERR;
   }

#endif

// w6 = (-8*a0+4*a1-2*a2+a3)*(-8*b0+4*b1-2*b2+b3)
//    = (+4*a1-2*a2+a3-8*a0)*(+4*b1-2*b2+b3-8*b0)
   if ((e = mp_mul_2d(&a1, 2, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a2, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &a3, &w6)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&a0, 3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_mul_2d(&b1, 2, &w7)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b2, 1, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w7, &tmp1, &w7)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w7, &b3, &w7)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_mul_2d(&b0, 3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w7, &tmp1, &w7)) != MP_OKAY) {
      goto ERR;
   }

#ifdef USE_OPEN_MP_NOT
#include <omp.h>
   #pragma omp parallel
   #pragma omp single nowait
   {
      #pragma omp  task shared(e1)
      e1 = mp_mul(&w6, &w7, &w6);
      #pragma omp  task shared(e2)
      e2 = mp_mul(&a0, &b0, &w7);
   }
   if ((e1 != MP_OKAY)||(e2 != MP_OKAY))
      goto ERR;
#else


   if ((e = mp_mul(&w6, &w7, &w6)) != MP_OKAY) {                 // <--
      goto ERR;
   }
// w7 = a0*b0
   if ((e = mp_mul(&a0, &b0, &w7)) != MP_OKAY) {                 // <--
      goto ERR;
   }
#endif
// w2 = w2 + w5;
   if ((e = mp_add(&w2, &w5, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w4 = w4 - w3;
   if ((e = mp_sub(&w4, &w3, &w4)) != MP_OKAY) {
      goto ERR;
   }
// w6 = w6 - w5;
   if ((e = mp_sub(&w6, &w5, &w6)) != MP_OKAY) {
      goto ERR;
   }
// w4 = w4 /2 ;
   if ((e = mp_div_2d(&w4, 1, &w4, NULL)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 - w1;
   if ((e = mp_sub(&w5, &w1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 - (64*w7) ;
   if ((e = mp_mul_2d(&w7, 6, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w3 = w3 + w4;
   if ((e = mp_add(&w3, &w4, &w3)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 *2 ;
   if ((e = mp_mul_2d(&w5, 1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 + w6;
   if ((e = mp_add(&w5, &w6, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 - (65*w3) ;
   if ((e = mp_mul_d(&w3, 65, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w3 = w3 - w1;
   if ((e = mp_sub(&w3, &w1, &w3)) != MP_OKAY) {
      goto ERR;
   }
// w3 = w3 - w7;
   if ((e = mp_sub(&w3, &w7, &w3)) != MP_OKAY) {
      goto ERR;
   }
// w4 = -w4;
   if ((e = mp_neg(&w4, &w4)) != MP_OKAY) {
      goto ERR;
   }
// w6 = -w6;
   if ((e = mp_neg(&w6, &w6)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 + (45*w3) ;
   if ((e = mp_mul_d(&w3, 45, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5 - (8*w3) ;
   if ((e = mp_mul_2d(&w3, 3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w5, &tmp1, &w5)) != MP_OKAY) {
      goto ERR;
   }
// w5 = w5/24;
   if ((e = mp_div_d(&w5, 24, &w5, NULL)) != MP_OKAY) {
      goto ERR;
   }

// w6 = w6 - w2;
   if ((e = mp_sub(&w6, &w2, &w6)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 - (16*w4) ;
   if ((e = mp_mul_2d(&w4, 4, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_sub(&w2, &tmp1, &w2)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2/18;
   if ((e = mp_div_d(&w2, 18, &w2, NULL)) != MP_OKAY) {
      goto ERR;
   }
// w3 = w3 - w5;
   if ((e = mp_sub(&w3, &w5, &w3)) != MP_OKAY) {
      goto ERR;
   }
// w4 = w4 - w2;
   if ((e = mp_sub(&w4, &w2, &w4)) != MP_OKAY) {
      goto ERR;
   }
// w6 = w6 + (30*w2) ;
   if ((e = mp_mul_d(&w2, 30, &tmp1)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&w6, &tmp1, &w6)) != MP_OKAY) {
      goto ERR;
   }
// w6 = w6/60;
   if ((e = mp_div_d(&w6, 60, &w6, NULL)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 - w6;
   if ((e = mp_sub(&w2, &w6, &w2)) != MP_OKAY) {
      goto ERR;
   }



#   if 0

   /*
     if ((e = mp_mul_2d(&w3, 6, &tmp1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_add(&w3, &tmp1, &tmp1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&w2, &tmp1, &w2)) != MP_OKAY) {
       goto ERR;
     }
   */
   mp_mul_d(&w3, 65, &tmp1);
   mp_sub(&w2, &tmp1, &w2);

// w5 = w5/24;
// all divisions are exact, so we can play with the prime factors
// or write a function to do exact division, which should do it faster
// w5 /= 24      = (w5 / 8) / 3 = (w5 >> 3) / 3
   /*
     if ((e = mp_div_2d(&w5, 3, &w5, NULL)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_div_3(&w5, &w5, NULL)) != MP_OKAY) {
       goto ERR;
     }
   */
   mp_div_d(&w5, 24, &w5, NULL);

// w2 = w2/18;
// w2 /= 18      = ((w2 >> 1) / 3 ) / 3
   /*
     if ((e = mp_div_2d(&w2, 1, &w2, NULL)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_div_3(&w2, &w2, NULL)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_div_3(&w2, &w2, NULL)) != MP_OKAY) {
       goto ERR;
     }
   */
   mp_div_d(&w2, 18, &w2, NULL);

// w6 += w2 * 30;
// w6 += (30*w2) = (w2 << 5) - (w2 << 1)
   /*
     if ((e = mp_mul_2d(&w2, 1, &tmp2)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_mul_2d(&tmp2, 4, &tmp1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_sub(&tmp1, &tmp2, &tmp1)) != MP_OKAY) {
       goto ERR;
     }
     if ((e = mp_add(&w6, &tmp1, &w6)) != MP_OKAY) {
       goto ERR;
     }
   */
   mp_mul_d(&w2, 30, &tmp1);
   mp_add(&w6, &tmp1, &w6);
// w6 = w6/60;
// w6 /= 60      = ((w6 >> 2) / 3 ) / 5
   if ((e = mp_div_d(&w6, 60, &w6, NULL)) != MP_OKAY) {
      goto ERR;
   }
// w2 = w2 - w6;
   if ((e = mp_sub(&w2, &w6, &w2)) != MP_OKAY) {
      goto ERR;
   }
#   endif
// S = w7 + w6*B + w5*B^2 + w4*B^3 + w3*B^4 + w2*B^5 + w1*B^6;

   if ((e = mp_copy(&w7, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w6, B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w6, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w5, 2 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w5, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w4, 3 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w4, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w3, 4 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w3, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w2, 5 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w2, &tmp1)) != MP_OKAY) {
      goto ERR;
   }

   if ((e = mp_lshd(&w1, 6 * B)) != MP_OKAY) {
      goto ERR;
   }
   if ((e = mp_add(&tmp1, &w1, c)) != MP_OKAY) {
      goto ERR;
   }

   c->sign = sign;

// print(S - a*b)
ERR:

ERRb3:
   mp_clear(&b3);
ERRb2:
   mp_clear(&b2);
ERRb1:
   mp_clear(&b1);
ERRb0:
   mp_clear(&b0);
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
//     &a0, &a1, &a2, &a3,&b0, &b1,&b2,&b3,
                  &tmp1, &tmp2, NULL);
   return e;
}
#endif
#endif

