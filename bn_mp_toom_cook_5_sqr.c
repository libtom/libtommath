#include <tommath.h>
#ifdef BN_MP_TOOM_COOK_5_SQR_C
extern int TOOM_COOK_5_SQR_CO;
int mp_toom_cook_5_sqr(mp_int * a, mp_int * c)
{
  mp_int w1, w2, w3, w4, w5, w6, w7, w8, w9;
  mp_int tmp1;
  mp_int a0, a1, a2, a3, a4;
  int e = MP_OKAY;
  int B, count, j;

  B = (a->used) / 4;

  if (a->used < TOOM_COOK_5_SQR_CO) {
    if ((e = mp_sqr(a, c)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }

  if ((e =
       mp_init_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7, &w8, &w9, &tmp1,
		     NULL)) != MP_OKAY) {
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
  if ((e = mp_init_size(&a4, B)) != MP_OKAY) {
    goto ERRa4;
  }
// A = a4*x^4+ a3*x^3 + a2*x^2 + a1*x + a0
  for (count = 0; count < B; count++) {
    a0.dp[count] = a->dp[count];
  }
  a0.used = B;
  for (; count < 2 * B; count++) {
    a1.dp[count - B] = a->dp[count];
  }
  a1.used = B;
  for (; count < 3 * B; count++) {
    a2.dp[count - 2 * B] = a->dp[count];
  }
  a2.used = B;
  for (; count < 4 * B; count++) {
    a3.dp[count - 3 * B] = a->dp[count];
  }
  a3.used = B;
  for (j = 0; count < a->used; count++, j++) {
    a4.dp[j] = a->dp[count];
  }
  a4.used = j;
  mp_clamp(&a0);mp_clamp(&a1);mp_clamp(&a2);mp_clamp(&a3);mp_clamp(&a4);
// S1 = a0^2
  if ((e = mp_sqr(&a0, &w1)) != MP_OKAY) {
    goto ERR;
  }
// S2 = a4^2
  if ((e = mp_sqr(&a4, &w2)) != MP_OKAY) {
    goto ERR;
  }
// S3 = (a0 + a1 + a2 + a3 + a4)^2
//    = (a0 + a1 + a3 +                 a2 + a4  )^2
  if ((e = mp_add(&a2, &a4, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S4 = (a0 - a1 + a2 - a3 + a4)^2
//    = (a0 - a1 - a3 +                 a2 + a4  )^2
  if ((e = mp_sub(&a0, &a1, &w4)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w4, &a3, &w4)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w4, &w3, &w4)) != MP_OKAY) {
    goto ERR;
  }
// add rest from above
  if ((e = mp_add(&w3, &a0, &w3)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w3, &a1, &w3)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w3, &a3, &w3)) != MP_OKAY) {
    goto ERR;
  }
// and finally square
  if ((e = mp_sqr(&w3, &w3)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sqr(&w4, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S5 = 2* (a0-a2+a4) * (a1-a3)
  if ((e = mp_sub(&a0, &a2, &w5)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w5, &a4, &w5)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&a1, &a3, &w6)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul(&w5, &w6, &w5)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&w5, 1, &w5)) != MP_OKAY) {
    goto ERR;
  }
// S6 = (a0 + a1 - a2 - a3 + a4) * (a0 - a1 - a2 + a3 + a4)
  if ((e = mp_add(&a0, &a1, &w6)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w6, &a2, &w6)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w6, &a3, &w6)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w6, &a4, &w6)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_sub(&a0, &a1, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w7, &a2, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w7, &a3, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w7, &a4, &w7)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_mul(&w6, &w7, &w6)) != MP_OKAY) {
    goto ERR;
  }
// S7 = (a1 + a2 - a4) * (a1 - a2 - a4 + 2*(a0-a3))
  if ((e = mp_add(&a1, &a2, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w7, &a4, &w7)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_sub(&a1, &a2, &w8)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w8, &a4, &w8)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_sub(&a0, &a3, &w9)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&w9, 1, &w9)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_add(&w8, &w9, &w8)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_mul(&w7, &w8, &w7)) != MP_OKAY) {
    goto ERR;
  }
// S8 = 2*a0*a1
  if ((e = mp_mul(&a0, &a1, &w8)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&w8, 1, &w8)) != MP_OKAY) {
    goto ERR;
  }
// S9 = 2*a3*a4
  if ((e = mp_mul(&a3, &a4, &w9)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&w9, 1, &w9)) != MP_OKAY) {
    goto ERR;
  }
// S4 = (S4+S3)/2
  if ((e = mp_add(&w4, &w3, &w4)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_div_2d(&w4, 1, &w4, NULL)) != MP_OKAY) {
    goto ERR;
  }
// S3 = S3-S4
  if ((e = mp_sub(&w3, &w4, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S6 = (S6+S4)/2
  if ((e = mp_add(&w6, &w4, &w6)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_div_2d(&w6, 1, &w6, NULL)) != MP_OKAY) {
    goto ERR;
  }
// S5 = (-S5+S3)/2
  if ((e = mp_sub(&w3, &w5, &w5)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_div_2d(&w5, 1, &w5, NULL)) != MP_OKAY) {
    goto ERR;
  }
// S4 = S4-S6
  if ((e = mp_sub(&w4, &w6, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S3 = S3-S5-S8
  if ((e = mp_sub(&w3, &w5, &w3)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w3, &w8, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S6 = S6-S2-S1
  if ((e = mp_sub(&w6, &w2, &w6)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w6, &w1, &w6)) != MP_OKAY) {
    goto ERR;
  }
// S5 = S5-S9
  if ((e = mp_sub(&w5, &w9, &w5)) != MP_OKAY) {
    goto ERR;
  }
// S7 = S7-S2-S8-S9+S6+S3
  if ((e = mp_sub(&w7, &w2, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w7, &w8, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w7, &w9, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w7, &w6, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w7, &w3, &w7)) != MP_OKAY) {
    goto ERR;
  }
// S4 = S4-S7
  if ((e = mp_sub(&w4, &w7, &w4)) != MP_OKAY) {
    goto ERR;
  }
// P = S2*x^8+ S9*x^7+ S4*x^6+ S3*x^5+ S6*x^4+ S5*x^3+ S7*x^2+ S8*x+ S1

  if ((e = mp_copy(&w1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w8, B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w8, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w7, 2 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w7, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w5, 3 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w5, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w6, 4 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w6, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w3, 5 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w4, 6 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w9, 7 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w9, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w2, 8 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w2, c)) != MP_OKAY) {
    goto ERR;
  }
ERR:

ERRa4:
  mp_clear(&a4);
ERRa3:
  mp_clear(&a3);
ERRa2:
  mp_clear(&a2);
ERRa1:
  mp_clear(&a1);
ERRa0:
  mp_clear(&a0);

ERR0:
  mp_clear_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7, &w8, &w9,
		 &tmp1, &a0, &a1, &a2, &a3, &a4, NULL);
  return e;
}

#endif
