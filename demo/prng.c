#include "shared.h"


/* Bob Jenkins' http://burtleburtle.net/bob/rand/smallprng.html */
/* Chosen for speed and a good "mix" */
typedef struct ranctx {
   uint64_t a;
   uint64_t b;
   uint64_t c;
   uint64_t d;
} ranctx;

static ranctx burtle_x;

#   define rot(x,k) (((x)<<(k))|((x)>>(64-(k))))
static uint64_t s_ranval(void)
{
   uint64_t e = burtle_x.a - rot(burtle_x.b, 7);
   burtle_x.a = burtle_x.b ^ rot(burtle_x.c, 13);
   burtle_x.b = burtle_x.c + rot(burtle_x.d, 37);
   burtle_x.c = burtle_x.d + e;
   burtle_x.d = e + burtle_x.a;
   return burtle_x.d;
}

static void s_raninit(uint64_t seed)
{
   uint64_t i;
   burtle_x.a = 0xf1ea5eed;
   burtle_x.b = burtle_x.c = burtle_x.d = seed;
   for (i = 0; i < 20; ++i) {
      (void) s_ranval();
   }
}


void s_random_seed(uint64_t seed)
{
   s_raninit(seed);
}

uint64_t s_random(void)
{
   return s_ranval();
}

/*
   The original used LTM's mp_rand which uses the cryptographically secure
   source of the OS for its purpose. That is too expensive, too slow and
   most important for a benchmark: it is not repeatable.
*/
int s_mp_random(mp_int *a, int limbs)
{
   int e = MP_OKAY;
   if ((e = mp_grow(a, limbs)) != MP_OKAY) {
      goto LTM_ERR;
   }
   a->used = limbs;
   limbs--;
   do {
      a->dp[limbs] = (mp_digit)(s_ranval() & MP_MASK);
   } while (limbs--);


LTM_ERR:
   return e;
}
