#include "tommath_private.h"
#ifdef MP_RADIX_SIZE_OVERESTIMATE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* returns size of ASCII representation */

/*
   Tables of {0, log_2([1..64])} times 2^p where p is the scale
   factor defined in LTM_RADIX_SIZE_SCALE.

   The tables have been computed with GP/PARI 2.9.4  with the
   x86-64/GMP-5.1.3 kernel and (unnecessary) 100 decimal digits
   precision. The fractional parts of the results after the
   multiplication by 2^p have been discarded (floor(x)).

   The number that measures the number of the digits of the bignum is
   of type "int" which is at least 16 bits large and is 32 bit on the
   architectures common at the time of writing. If you have hard- or
   software that has a different size, please feel free to contact the
   author, adding another table can be done quite quickly!
 */
/* *INDENT-OFF* */
#if (defined MP_16BIT)
#define LTM_RADIX_SIZE_SCALE 61
#define LTM_RADIX_SIZE_HALF_TABLE_SCALE 32
static const mp_word logbases_high[65] = {
            0ul,          0ul,  536870912ul,  850920263ul, 1073741824ul,
   1246575653ul, 1387791175ul, 1507187197ul, 1610612736ul, 1701840526ul,
   1783446565ul, 1857268208ul, 1924662087ul, 1986658446ul, 2044058109ul,
   2097495917ul, 2147483648ul, 2194439903ul, 2238711438ul, 2280588718ul,
   2320317477ul, 2358107460ul, 2394139120ul, 2428568832ul, 2461532999ul,
   2493151307ul, 2523529358ul, 2552760789ul, 2580929021ul, 2608108687ul,
   2634366829ul, 2659763891ul, 2684354560ul, 2708188471ul, 2731310815ul,
   2753762851ul, 2775582350ul, 2796803979ul, 2817459630ul, 2837578709ul,
   2857188389ul, 2876313830ul, 2894978372ul, 2913203707ul, 2931010032ul,
   2948416180ul, 2965439744ul, 2982097182ul, 2998403911ul, 3014374394ul,
   3030022219ul, 3045360166ul, 3060400270ul, 3075153878ul, 3089631701ul,
   3103843862ul, 3117799933ul, 3131508981ul, 3144979599ul, 3158219938ul,
   3171237741ul, 3184040363ul, 3196634803ul, 3209027723ul, 3221225472ul
};
static const mp_word logbases_low[65] = {
            0ul,          0ul,          0ul, 1065013491ul,          0ul,
   3868050815ul, 1065013491ul, 1343271782ul,          0ul, 2130026983ul,
   3868050815ul,  427100031ul, 1065013491ul, 1200147405ul, 1343271782ul,
    638097011ul,          0ul, 1495425743ul, 2130026983ul, 1083835016ul,
   3868050815ul, 2408285274ul,  427100031ul, 3594167896ul, 1065013491ul,
   3441134334ul, 1200147405ul, 3195040475ul, 1343271782ul, 1429396043ul,
    638097011ul, 1651143338ul,          0ul, 1492113523ul, 1495425743ul,
    916355301ul, 2130026983ul, 1832592635ul, 1083835016ul, 2265160897ul,
   3868050815ul, 3464916779ul, 2408285274ul, 2621526098ul,  427100031ul,
   1703110503ul, 3594167896ul, 2513440227ul, 1065013491ul, 2686543564ul,
   3441134334ul, 2560439235ul, 1200147405ul, 1309103046ul, 3195040475ul,
       183550ul, 1343271782ul, 2148848508ul, 1429396043ul, 3785381171ul,
    638097011ul, 1073048670ul, 1651143338ul, 3473298766ul,          0ul
};
/* TODO: This assumes the type "long long" exists. If it doesn't: use the tables for MP_16BIT */
#elif ( (defined MP_28BIT) || (defined MP_31BIT) || (defined MP_32BIT)  || (defined MP_64BIT) )
#define LTM_RADIX_SIZE_SCALE 61
#ifdef MP_64BIT
static const mp_digit logbases[65] = {
#else
static const mp_word logbases[65] = {
#endif
                      0ull,                    0ull,  2305843009213693952ull,
    3654674702153732339ull,  4611686018427387904ull,  5354001665492895103ull,
    5960517711367426291ull,  6473319721408181094ull,  6917529027641081856ull,
    7309349404307464679ull,  7659844674706589055ull,  7976906213687625599ull,
    8266360720581120243ull,  8532633055092329421ull,  8779162730621875046ull,
    9008676367646627443ull,  9223372036854775808ull,  9425047617917838031ull,
    9615192413521158631ull,  9795053960520401544ull,  9965687683920283007ull,
   10127994423561913434ull, 10282749222901319551ull, 10430623713119086168ull,
   10572203729794814195ull, 10708003330985790206ull, 10838476064306023373ull,
   10964024106461197019ull, 11085005739835568998ull, 11201741516507896395ull,
   11314519376860321395ull, 11423598928577852074ull, 11529215046068469760ull,
   11631580915841357939ull, 11730890627131531983ull, 11827321386901076197ull,
   11921035422734852583ull, 12012181624960263419ull, 12100896969734095496ull,
   12187307757246061761ull, 12271530693133976959ull, 12353673836347420459ull,
   12433837432775607386ull, 12512114650772492370ull, 12588592232115013503ull,
   12663351069800359783ull, 12736466722332780120ull, 12808009872697200099ull,
   12878046739008508147ull, 12946639442816362188ull, 13013846340199484158ull,
   13079722320071570371ull, 13144319073519717325ull, 13207685337486676934ull,
   13269867115674890971ull, 13330907879180520702ull, 13390848749049262950ull,
   13449728662674133884ull, 13507584525721590347ull, 13564451351070528819ull,
   13620362386074015347ull, 13675349229302017118ull, 13729441937791546026ull,
   13782669125715645774ull, 13835058055282163712ull
};
#else
#  ifdef _MSC_VER
#    pragma message("mp_radix_size_overestimate: unknown or undefined MP_xBIT type")
#  else
#    warning "mp_radix_size_overestimate: unknown or undefined MP_xBIT type"
#  endif
#endif
/* *INDENT-ON* */

mp_err mp_radix_size_overestimate(const mp_int *a, const int radix, size_t *size)
{
   mp_int bi_bit_count, bi_k;
#ifdef MP_16BIT
   mp_int bi_k_bis;
#endif
   int bit_count;
   mp_err err = MP_OKAY;

   *size = 0u;

   if ((radix < 2) || (radix > 64)) {
      return MP_VAL;
   }

   if (a->used == 0) {
      *size = 2u;
      return MP_OKAY;
   }

   bit_count = mp_count_bits(a);

   /* A small shortcut for powers of two. */
   if (!(radix&(radix-1))) {
      unsigned int x = (unsigned int)radix;
      int y, t;
      for (y=0; (y < 7) && !(x & 1u); y++) {
         x >>= 1;
      }
      t = bit_count/y;
      bit_count  = bit_count - (t * y);
      /* Add 1 for the remainder if any and 1 for "\0". */
      t += ((bit_count  == 0) ? 1 : 2) + (a->sign == MP_NEG);
      *size = (size_t)t;
      return MP_OKAY;
   }

   /* d(bitcount,radix) = floor( (bitcount * 2^p) / k(radix) ) + 1 */

   if ((err = mp_init_multi(&bi_bit_count, &bi_k, NULL)) != MP_OKAY) {
      return err;
   }
#ifdef MP_16BIT
   if ((err = mp_init(&bi_k_bis)) != MP_OKAY) {
      /* No "goto" to avoid cluttering this code with even more preprocessor branches */
      mp_clear_multi(&bi_bit_count, &bi_k, NULL);
      return err;
   }
#endif
   /* "long" is at least as large as "int" according to even the oldest C standards */
   mp_set_l(&bi_bit_count, bit_count);
#ifdef MP_16BIT
   mp_set_u32(&bi_k, logbases_high[radix]);
   if ((err = mp_mul_2d(&bi_k, LTM_RADIX_SIZE_HALF_TABLE_SCALE, &bi_k)) != MP_OKAY) {
      goto LTM_ERR;
   }
   mp_set_u32(&bi_k_bis, logbases_low[radix]);
   if ((err = mp_add(&bi_k_bis, &bi_k, &bi_k)) != MP_OKAY) {
      goto LTM_ERR;
   }
#else
   mp_set_u64(&bi_k, logbases[radix]);
#endif
   if ((err = mp_mul_2d(&bi_bit_count, LTM_RADIX_SIZE_SCALE, &bi_bit_count)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((err = mp_div(&bi_bit_count, &bi_k, &bi_bit_count, NULL)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /*
      The first addition of one is done because of the truncating division,
      and the second addition of one is for NIL ('\0').

      Casting from "long" to "int" can be done because "bi_bit_count" fits into an "int"
      by definition.
    */
   *size = (size_t)mp_get_l(&bi_bit_count) + 1 + 1 + (a->sign == MP_NEG);

LTM_ERR:
   mp_clear_multi(&bi_bit_count, &bi_k, NULL);
#ifdef MP_16BIT
   mp_clear(&bi_k_bis);
#endif
   return err;
}



#endif
