#include <tommath.h>
#ifdef BN_MP_TOOM_COOK_5_MUL_C
extern int TOOM_COOK_5_MUL_CO;
int mp_toom_cook_5_mul(mp_int * a, mp_int * b, mp_int * c)
{
  mp_int w1, w2, w3, w4, w5, w6, w7, w8, w9;
  mp_int tmp1, tmp2;
  mp_int a0, a1, a2, a3, a4;
  mp_int b0, b1, b2, b3, b4;
  int e = MP_OKAY;
  int B, count, sign;

  B = (MAX(a->used, b->used)) / 5;


  sign = (a->sign != b->sign) ? MP_NEG : MP_ZPOS;
  if (MIN(a->used, b->used) < TOOM_COOK_5_MUL_CO) {
    if ((e = mp_mul(a, b, c)) != MP_OKAY) {
      return e;
    }
    c->sign = sign;
    return MP_OKAY;
  }



  if ((e =
       mp_init_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7, &w8, &w9, &tmp1, &tmp2,
		     //&a0, &a1, &a2, &a3, &a4, &b0, &b1, &b2, &b3, &b4,
		     NULL)) != MP_OKAY) {
    goto ERR0;
    //goto ERR;
  }

  if ((e = mp_init_size(&a0, B )) != MP_OKAY) {
    goto ERRa0;
  }
  if ((e = mp_init_size(&a1, B )) != MP_OKAY) {
    goto ERRa1;
  }
  if ((e = mp_init_size(&a2, B )) != MP_OKAY) {
    goto ERRa2;
  }
  if ((e = mp_init_size(&a3, B )) != MP_OKAY) {
    goto ERRa3;
  }
  if ((e = mp_init_size(&a4, B )) != MP_OKAY) {
    goto ERRa4;
  }

  if ((e = mp_init_size(&b0, B )) != MP_OKAY) {
    goto ERRb0;
  }
  if ((e = mp_init_size(&b1, B )) != MP_OKAY) {
    goto ERRb1;
  }
  if ((e = mp_init_size(&b2, B )) != MP_OKAY) {
    goto ERRb2;
  }
  if ((e = mp_init_size(&b3, B )) != MP_OKAY) {
    goto ERRb3;
  }
  if ((e = mp_init_size(&b4, B )) != MP_OKAY) {
    goto ERRb4;
  }
  // A = a4*x^4 + a3*x^3 + a2*x^2 + a1*x + a0
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
    case 4:
      a4.dp[count - 4 * B] = a->dp[count];
      a4.used++;
      break;
    default:
      a4.dp[count - 4 * B] = a->dp[count];
      a4.used++;
      break;
    }
  }
  // B = b4*x^4 + b3*x^3 + b2*x^2 + b1*x + b0
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
    case 4:
      b4.dp[count - 4 * B] = b->dp[count];
      b4.used++;
      break;
    default:
      b4.dp[count - 4 * B] = b->dp[count];
      b4.used++;
      break;
    }
  }



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
  mp_rshd(&a2, B * 2);
  mp_mod_2d(&a2, DIGIT_BIT * B, &a2);

  if ((e = mp_copy(a, &a3)) != MP_OKAY) {
    goto ERR;
  }
  mp_rshd(&a3, B * 3);

  mp_mod_2d(&a3, DIGIT_BIT * B, &a3);

  if ((e = mp_copy(a, &a4)) != MP_OKAY) {
    goto ERR;
  }
  mp_rshd(&a4, B * 4);



  if ((e = mp_mod_2d(b, DIGIT_BIT * B, &b0)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_copy(a, &b1)) != MP_OKAY) {
    goto ERR;
  }
  mp_rshd(&b1, B);
  mp_mod_2d(&b1, DIGIT_BIT * B, &b1);

  if ((e = mp_copy(b, &b2)) != MP_OKAY) {
    goto ERR;
  }
  mp_rshd(&b2, B * 2);
  mp_mod_2d(&b2, DIGIT_BIT * B, &b2);

  if ((e = mp_copy(b, &b3)) != MP_OKAY) {
    goto ERR;
  }
  mp_rshd(&b3, B * 3);

  mp_mod_2d(&b3, DIGIT_BIT * B, &b3);

  if ((e = mp_copy(b, &b4)) != MP_OKAY) {
    goto ERR;
  }
  mp_rshd(&b4, B * 4);
*/

// S1 = a4*b4
  if ((e = mp_toom_cook_5_mul(&a4, &b4, &w1)) != MP_OKAY) {
    goto ERR;
  }
// S9 = a0*b0
  if ((e = mp_toom_cook_5_mul(&a0, &b0, &w9)) != MP_OKAY) {
    goto ERR;
  }
// S2 = (a0- 2*a1 +4*a2 -8*a3 +16*a4)

  if ((e = mp_mul_2d(&a1, 1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }				// 2*a1     = tmp1
  if ((e = mp_sub(&a0, &tmp1, &w2)) != MP_OKAY) {
    goto ERR;
  }				// a0- 2*a1 = a0 - tmp1 = w2
  if ((e = mp_mul_2d(&a2, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }				// 4*a2     = tmp1
  if ((e = mp_add(&w2, &tmp1, &w2)) != MP_OKAY) {
    goto ERR;
  }				// a0- 2*a1 +4*a2 = w2 + tmp1 = w2
  if ((e = mp_mul_2d(&a3, 3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }				// 8*a3     = tmp1
  if ((e = mp_sub(&w2, &tmp1, &w2)) != MP_OKAY) {
    goto ERR;
  }				// a0- 2*a1 +4*a2 -8*a3 = w2 - tmp1 = w2
  if ((e = mp_mul_2d(&a4, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }				// 16*a4     = tmp1
  if ((e = mp_add(&w2, &tmp1, &w2)) != MP_OKAY) {
    goto ERR;
  }				// a0- 2*a1 +4*a2 -8*a3 +16*a4 = w2 + tmp1 = w2

//    * (b0- 2*b1 +4*b2 -8*b3 +16*b4)
  if ((e = mp_mul_2d(&b1, 1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&b0, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b2, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b3, 3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b4, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_toom_cook_5_mul(&tmp2, &w2, &w2)) != MP_OKAY) {
    goto ERR;
  }
// S5 = (a0+ 2*a1+ 4*a2+ 8*a3+ 16*a4)
  if ((e = mp_mul_2d(&a1, 1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&a0, &tmp1, &w5)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a2, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a3, 3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a4, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
    goto ERR;
  }
// *(b0+ 2*b1+ 4*b2+ 8*b3+ 16*b4)
  if ((e = mp_mul_2d(&b1, 1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&b0, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b2, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b3, 3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b4, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_toom_cook_5_mul(&tmp2, &w5, &w5)) != MP_OKAY) {
    goto ERR;
  }
// S3 = (a4+ 2*a3+ 4*a2+ 8*a1+ 16*a0)
  if ((e = mp_mul_2d(&a3, 1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&a4, &tmp1, &w3)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a2, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w3, &tmp1, &w3)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a1, 3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w3, &tmp1, &w3)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a0, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w3, &tmp1, &w3)) != MP_OKAY) {
    goto ERR;
  }
// *    (b4+ 2*b3+ 4*b2+ 8*b1+ 16*b0)
  if ((e = mp_mul_2d(&b3, 1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&b4, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b2, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b1, 3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b0, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_toom_cook_5_mul(&tmp2, &w3, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S8 = (a4- 2*a3+ 4*a2- 8*a1+ 16*a0)
  if ((e = mp_mul_2d(&a3, 1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&a4, &tmp1, &w8)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a2, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w8, &tmp1, &w8)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a1, 3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w8, &tmp1, &w8)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a0, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w8, &tmp1, &w8)) != MP_OKAY) {
    goto ERR;
  }
//*     (b4- 2*b3+ 4*b2- 8*b1+ 16*b0)
  if ((e = mp_mul_2d(&b3, 1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&b4, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b2, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b1, 3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b0, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_toom_cook_5_mul(&tmp2, &w8, &w8)) != MP_OKAY) {
    goto ERR;
  }
// S4 = (a0+ 4*a1+ 16*a2+ 64*a3+ 256*a4)
  if ((e = mp_mul_2d(&a1, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&a0, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a2, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w4, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a3, 6, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w4, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&a4, 8, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w4, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
//*     (b0+ 4*b1+ 16*b2+ 64*b3+ 256*b4)
  if ((e = mp_mul_2d(&b1, 2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&b0, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b2, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b3, 6, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_mul_2d(&b4, 8, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp2, &tmp1, &tmp2)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_toom_cook_5_mul(&tmp2, &w4, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S6 = (a0- a1+ a2- a3 +a4)
  if ((e = mp_sub(&a0, &a1, &w6)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w6, &a2, &w6)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w6, &a3, &w6)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w6, &a4, &w6)) != MP_OKAY) {
    goto ERR;
  }
// *    (b0- b1+ b2- b3+ b4)
  if ((e = mp_sub(&b0, &b1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &b2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&tmp1, &b3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &b4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_toom_cook_5_mul(&tmp1, &w6, &w6)) != MP_OKAY) {
    goto ERR;
  }
// S7 = (a0+ a1+ a2+ a3+ a4)
  if ((e = mp_add(&a0, &a1, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w7, &a2, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w7, &a3, &w7)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w7, &a4, &w7)) != MP_OKAY) {
    goto ERR;
  }
// *    (b0+ b1+ b2+ b3+ b4)
  if ((e = mp_add(&b0, &b1, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &b2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &b3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &b4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_toom_cook_5_mul(&tmp1, &w7, &w7)) != MP_OKAY) {
    goto ERR;
  }
// S6 -= S7
  if ((e = mp_sub(&w6, &w7, &w6)) != MP_OKAY) {
    goto ERR;
  }
// S2 -= S5
  if ((e = mp_sub(&w2, &w5, &w2)) != MP_OKAY) {
    goto ERR;
  }
// S4 -= S9
  if ((e = mp_sub(&w4, &w9, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S4 -= (2^16*S1)
  if ((e = mp_mul_2d(&w1, 16, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w4, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S8 -= S3
  if ((e = mp_sub(&w8, &w3, &w8)) != MP_OKAY) {
    goto ERR;
  }
// S6 /= 2
  if ((e = mp_div_2d(&w6, 1, &w6, NULL)) != MP_OKAY) {
    goto ERR;
  }
// S5 *= 2
  if ((e = mp_mul_2d(&w5, 1, &w5)) != MP_OKAY) {
    goto ERR;
  }
// S5 += S2
  if ((e = mp_add(&w5, &w2, &w5)) != MP_OKAY) {
    goto ERR;
  }
// S2 = -S2
  if ((e = mp_neg(&w2, &w2)) != MP_OKAY) {
    goto ERR;
  }
// S8 = -S8
  if ((e = mp_neg(&w8, &w8)) != MP_OKAY) {
    goto ERR;
  }
// S7 += S6
  if ((e = mp_add(&w7, &w6, &w7)) != MP_OKAY) {
    goto ERR;
  }
// S6 = -S6
  if ((e = mp_neg(&w6, &w6)) != MP_OKAY) {
    goto ERR;
  }
// S3 -= S7
  if ((e = mp_sub(&w3, &w7, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S5 -= (512*S7)
  if ((e = mp_mul_2d(&w7, 9, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w5, &tmp1, &w5)) != MP_OKAY) {
    goto ERR;
  }
// S3 *= 2
  if ((e = mp_mul_2d(&w3, 1, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S3 -= S8
  if ((e = mp_sub(&w3, &w8, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S7 -= S1
  if ((e = mp_sub(&w7, &w1, &w7)) != MP_OKAY) {
    goto ERR;
  }
// S7 -= S9
  if ((e = mp_sub(&w7, &w9, &w7)) != MP_OKAY) {
    goto ERR;
  }
// S8 += S2
  if ((e = mp_add(&w8, &w2, &w8)) != MP_OKAY) {
    goto ERR;
  }
// S5 += S3
  if ((e = mp_add(&w5, &w3, &w5)) != MP_OKAY) {
    goto ERR;
  }
// S8 -= (80*S6)
  if ((e = mp_mul_d(&w6, 80, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w8, &tmp1, &w8)) != MP_OKAY) {
    goto ERR;
  }
// S3 -= (510*S9)
  if ((e = mp_mul_d(&w9, 510, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w3, &tmp1, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S4 -= S2
  if ((e = mp_sub(&w4, &w2, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S3 *= 3
  if ((e = mp_mul_d(&w3, 3, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S3 += S5
  if ((e = mp_add(&w3, &w5, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S8 /= 180 \\ division by 180
  if ((e = mp_div_d(&w8, 180, &w8, NULL)) != MP_OKAY) {
    goto ERR;
  }
// S5 += (378*S7)
  if ((e = mp_mul_d(&w7, 378, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w5, &tmp1, &w5)) != MP_OKAY) {
    goto ERR;
  }
// S2 /= 4
  if ((e = mp_div_2d(&w2, 2, &w2, NULL)) != MP_OKAY) {
    goto ERR;
  }
// S6 -= S2
  if ((e = mp_sub(&w6, &w2, &w6)) != MP_OKAY) {
    goto ERR;
  }
// S5 /= (-72) \\ division by -72
  if ((e = mp_div_d(&w5, 72, &w5, NULL)) != MP_OKAY) {
    goto ERR;
  }
  if (&w5.sign == MP_ZPOS)
    (&w5)->sign = MP_NEG;
  (&w5)->sign = MP_ZPOS;
// S3 /= (-360) \\ division by -360
  if ((e = mp_div_d(&w3, 360, &w3, NULL)) != MP_OKAY) {
    goto ERR;
  }
  if (&w3.sign == MP_ZPOS)
    (&w3)->sign = MP_NEG;
  (&w3)->sign = MP_ZPOS;
// S2 -= S8
  if ((e = mp_sub(&w2, &w8, &w2)) != MP_OKAY) {
    goto ERR;
  }
// S7 -= S3
  if ((e = mp_sub(&w7, &w3, &w7)) != MP_OKAY) {
    goto ERR;
  }
// S4 -= (256*S5)
  if ((e = mp_mul_2d(&w5, 8, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w4, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S3 -= S5
  if ((e = mp_sub(&w3, &w5, &w3)) != MP_OKAY) {
    goto ERR;
  }
// S4 -= (4096*S3)
  if ((e = mp_mul_2d(&w3, 12, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w4, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S4 -= (16*S7)
  if ((e = mp_mul_2d(&w7, 4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_sub(&w4, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S4 += (256*S6)
  if ((e = mp_mul_2d(&w6, 8, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w4, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S6 += S2
  if ((e = mp_add(&w6, &w2, &w6)) != MP_OKAY) {
    goto ERR;
  }
// S2 *= 180
  if ((e = mp_mul_d(&w2, 180, &w2)) != MP_OKAY) {
    goto ERR;
  }
// S2 += S4
  if ((e = mp_add(&w2, &w4, &w2)) != MP_OKAY) {
    goto ERR;
  }
// S2 /= 11340 \\ division by 11340
  if ((e = mp_div_d(&w2, 11340, &w2, NULL)) != MP_OKAY) {
    goto ERR;
  }
// S4 += (720*S6)
  if ((e = mp_mul_d(&w6, 720, &tmp1)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&w4, &tmp1, &w4)) != MP_OKAY) {
    goto ERR;
  }
// S4 /= (-2160) \\ division by -2160
  if ((e = mp_div_d(&w4, 2160, &w4, NULL)) != MP_OKAY) {
    goto ERR;
  }
  if (&w4.sign == MP_ZPOS)
    (&w4)->sign = MP_NEG;
  (&w4)->sign = MP_ZPOS;
// S6 -= S4
  if ((e = mp_sub(&w6, &w4, &w6)) != MP_OKAY) {
    goto ERR;
  }
// S8 -= S2
  if ((e = mp_sub(&w8, &w2, &w8)) != MP_OKAY) {
    goto ERR;
  }
// P = S1*x^8 + S2*x^7 + S3*x^6 + S4*x^5 + S5*x^4 + S6*x^3 + S7*x^2 + S8*x + S9
  if ((e = mp_copy(&w9, &tmp1)) != MP_OKAY) {
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

  if ((e = mp_lshd(&w6, 3 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w6, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w5, 4 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w5, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w4, 5 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w4, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w3, 6 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w3, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w2, 7 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w2, &tmp1)) != MP_OKAY) {
    goto ERR;
  }

  if ((e = mp_lshd(&w1, 8 * B)) != MP_OKAY) {
    goto ERR;
  }
  if ((e = mp_add(&tmp1, &w1, c)) != MP_OKAY) {
    goto ERR;
  }
// P - A*B \\ == zero 
  c->sign = sign;
ERR:

ERRb4:
  mp_clear(&b4);
ERRb3:
  mp_clear(&b3);
ERRb2:
  mp_clear(&b2);
ERRb1:
  mp_clear(&b1);
ERRb0:
  mp_clear(&b0);
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

  mp_clear_multi(&w1, &w2, &w3, &w4, &w5, &w6, &w7, &w8, &w9, &tmp1, &tmp2,
		// &a0, &a1, &a2, &a3, &a4, &b0, &b1, &b2, &b3, &b4,
                 NULL);
  return e;
}

#endif

