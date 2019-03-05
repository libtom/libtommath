#include "tommath_private.h"
#ifdef BN_MP_RADIX_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * SPDX-License-Identifier: Unlicense
 */

/* returns size of ASCII representation */
#if ( (defined LTM_USE_FASTER_VERSIONS) || (defined LTM_USE_FASTER_RADIX_SIZE))
/*
   Overestimate the amount of memory needed for the number-to-string conversions.
   The results for powers-of-two are exact but it will overestimate the actual
   amount by up to 1 character for all other bases with the exception of MP_16BIT
   where the overestimate is up to to 2 characters for all those other bases if
   the "int" type has more than 16 bits.
*/

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

   A table for a 64 bit large "int" is already included but commented out.
 */

/*
   All table entries are of the type "mp_word" with the exception of the one for
   MP-64BIT. We use the macro
      MP_SET_XLONG(func_name, type)
   defined in tommath_private.h to ease our workload.

   The following two macros expand to:

      static int s_set_mp_digit(mp_int *a, mp_digit b) {...}

   and

      static int s_set_mp_word(mp_int *a, mp_word b) {...}

   respectively
 */
#ifdef MP_64BIT
/* We cannot use mp_set() because it uses only DIGIT_BIT = 60 but need 61 bits */
static MP_SET_XLONG(s_set_mp_digit, mp_digit)
#else
static MP_SET_XLONG(s_set_mp_word, mp_word)
#endif
/*
   We do not really know how large/small "int" is and "mp_set_int()" uses
   "long", a type that might not be native.

   It expands to:

      static int s_set_int(mp_int *a, int b) {...}
*/
static MP_SET_XLONG(s_set_int, int)
/*
   With the same reasoning as above...
 */
static int s_get_int(const mp_int *a)
{
   int int_size, digit_size, digits_needed;
   unsigned int res;
   mp_word tmp;

   if (a->used == 0) {
      return 0;
   }

   int_size = ((int)sizeof(int) * CHAR_BIT);
   digit_size = ((int)sizeof(mp_digit) * CHAR_BIT);

   if (int_size <= digit_size) {
      res = (unsigned int) DIGIT(a, 0);
   } else {
      digits_needed = MIN(a->used, ((int_size + DIGIT_BIT - 1) / DIGIT_BIT)) - 1;
      tmp = DIGIT(a, digits_needed);
      while (--digits_needed >= 0) {
         tmp = (tmp << DIGIT_BIT) | DIGIT(a, digits_needed);
      }
      res = (unsigned int)tmp;
   }
   /*
      Casting "unsigned int" to the smaller "int" without further ado
      is possible because all input is guaranteed to fit into an "int".
    */
   return (int) res;
}

/* *INDENT-OFF* */
#ifdef MP_8BIT
#define LTM_RADIX_SIZE_SCALE 29
#define LTM_RADIX_SIZE_HALF_TABLE_SCALE 16
static const mp_word logbases_high[65] = {
       0u,     0u,  8192u, 12984u, 16384u,
   19021u, 21176u, 22997u, 24576u, 25968u,
   27213u, 28339u, 29368u, 30314u, 31189u,
   32005u, 32768u, 33484u, 34160u, 34799u,
   35405u, 35981u, 36531u, 37057u, 37560u,
   38042u, 38506u, 38952u, 39381u, 39796u,
   40197u, 40584u, 40960u, 41323u, 41676u,
   42019u, 42352u, 42675u, 42991u, 43298u,
   43597u, 43889u, 44173u, 44451u, 44723u,
   44989u, 45249u, 45503u, 45752u, 45995u,
   46234u, 46468u, 46698u, 46923u, 47144u,
   47360u, 47573u, 47783u, 47988u, 48190u,
   48389u, 48584u, 48776u, 48965u, 49152u
};
static const mp_word logbases_low[65] = {
       0u,     0u,     0u,   839u,     0u,
   15397u,   839u, 55805u,     0u,  1678u,
   15397u, 43504u,   839u,   142u, 55805u,
   16237u,     0u, 32479u,  1678u,  1454u,
   15397u, 56644u, 43504u,  1280u,   839u,
   30795u,   142u,  2517u, 55805u, 38031u,
   16237u, 50867u,     0u, 44343u, 32479u,
    5667u,  1678u, 55179u,  1454u,   981u,
   15397u,  4326u, 56644u, 62971u, 43504u,
   17076u,  1280u, 12574u,   839u, 46074u,
   30795u, 33318u,   142u,  8150u,  2517u,
   58902u, 55805u,  2293u, 38031u, 40098u,
   16237u, 39339u, 50867u, 57483u,     0u
};
#elif (defined MP_16BIT)
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

/* TODO: This assumes the type "long long" exists. If it doesn't use the tables for MP_16BIT */
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
#    pragma message("mp_estimate_string_size: unknown or undefined MP_xBIT type")
#  else
#    warning "mp_estimate_string_size: unknown or undefined MP_xBIT type"
#  endif
#endif
#if 0
/*
   Tables for 64 bit large "int".
   We have to use two tables here because there is no standard suffix for
   128-bit literals.
 */
#define LTM_RADIX_SIZE_SCALE 125
#define LTM_RADIX_SIZE_HALF_TABLE_SCALE 64
static const mp_digit logbases_low[65] = {
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
static const mp_digit logbases_high[65] = {
                      0ull,                    0ull,                    0ull,
   17582442452630992314ull,                    0ull,  2636289682573880742ull,
   17582442452630992314ull,  7302903955535475976ull,                    0ull,
   16718140831552433012ull,  2636289682573880742ull,  2260539591812094679ull,
   17582442452630992314ull,  3376366635243825699ull,  7302903955535475976ull,
    1771988061495321440ull,                    0ull, 11063778906989063000ull,
   16718140831552433012ull, 16876788983675997564ull,  2636289682573880742ull,
    6438602334456916675ull,  2260539591812094679ull,  9392066375228186965ull,
   17582442452630992314ull,  5272579365147761484ull,  3376366635243825699ull,
   15853839210473873710ull,  7302903955535475976ull,  5436200125372755171ull,
    1771988061495321440ull,  2424000858589553290ull,                    0ull,
    1396237970733535378ull, 11063778906989063000ull,  9939193638109356719ull,
   16718140831552433012ull, 17890464788716031277ull, 16876788983675997564ull,
    2512065014165266397ull,  2636289682573880742ull, 14314979085988782625ull,
    6438602334456916675ull,   736193948012496726ull,  2260539591812094679ull,
     907686440416762138ull,  9392066375228186965ull, 13047959311228526620ull,
   17582442452630992314ull, 14605807911070951953ull,  5272579365147761484ull,
   10199477285910503698ull,  3376366635243825699ull, 16535036200126848317ull,
   15853839210473873710ull,  4896829274385975422ull,  7302903955535475976ull,
   16012487362597438262ull,  5436200125372755171ull,  7605258474266570783ull,
    1771988061495321440ull,  7438463155377325683ull,  2424000858589553290ull,
    5574300713378357373ull,                    0ull
};

#endif
/* *INDENT-ON* */

int mp_radix_size(const mp_int *a, const int base, int *size)
{
   mp_int bi_bit_count, bi_k;
#if ( (defined MP_8BIT) || (defined MP_16BIT) )
   mp_int bi_k_bis;
#endif
   int ln2 = 0, rem, bit_count;
   int e = MP_OKAY;

   if ((base < 2) || (base > 64)) {
      *size = 0;
      return MP_VAL;
   }

   /* floor( log_2(a) ) + 1 */
   bit_count = mp_count_bits(a) + 1;

   if (bit_count == 0) {
      *size = 2;
      return MP_OKAY;
   }
   if (base == 2) {
      *size = bit_count + 1;
      return MP_OKAY;
   }

   /* A small shortcut for powers of two. */
   switch (base) {
   case 4:
      ln2 = 2;
      break;
   case 8:
      ln2 = 3;
      break;
   case 16:
      ln2 = 4;
      break;
   case 32:
      ln2 = 5;
      break;
   case 64:
      ln2 = 6;
      break;
   default:
      break;
   }
   if (ln2 != 0) {
      *size = bit_count/ln2;
      rem = bit_count - (*size) * ln2;
      /* Add 1 for the remainder if any and 1 for "\0". */
      *size += (rem == 0) ? 1 : 2;
      return MP_OKAY;
   }

   /* d(bitcount,base) = floor( (bitcount * 2^p) / k(base) ) + 1 */

   if ((e = mp_init_multi(&bi_bit_count, &bi_k, NULL)) != MP_OKAY) {
      *size = 0;
      return e;
   }
#if ( (defined MP_8BIT) || (defined MP_16BIT) )
   if ((e = mp_init(&bi_k_bis)) != MP_OKAY) {
      /* No "goto" to avoid cluttering this code with even more preprocessor branches */
      mp_clear_multi(&bi_bit_count, &bi_k, NULL);
      *size = 0;
      return e;
   }
#endif
   if ((e = s_set_int(&bi_bit_count, bit_count)) != MP_OKAY) {
      *size = 0;
      goto LTM_E1;
   }
#if ( (defined MP_8BIT) || (defined MP_16BIT) )
   if ((e = s_set_mp_word(&bi_k, logbases_high[base])) != MP_OKAY) {
      *size = 0;
      goto LTM_E1;
   }
   if ((e = mp_mul_2d(&bi_k, LTM_RADIX_SIZE_HALF_TABLE_SCALE, &bi_k)) != MP_OKAY) {
      *size = 0;
      goto LTM_E1;
   }
   if ((e = s_set_mp_word(&bi_k_bis, logbases_low[base])) != MP_OKAY) {
      *size = 0;
      goto LTM_E1;
   }
   if ((e = mp_add(&bi_k_bis, &bi_k, &bi_k)) != MP_OKAY) {
      *size = 0;
      goto LTM_E1;
   }
#elif (defined MP_64BIT)
   if ((e = s_set_mp_digit(&bi_k, logbases[base])) != MP_OKAY) {
      *size = 0;
      goto LTM_E1;
   }
#else
   if ((e = s_set_mp_word(&bi_k, logbases[base])) != MP_OKAY) {
      *size = 0;
      goto LTM_E1;
   }
#endif
   if ((e = mp_mul_2d(&bi_bit_count,LTM_RADIX_SIZE_SCALE , &bi_bit_count)) != MP_OKAY) {
      *size = 0;
      goto LTM_E1;
   }
   if ((e = mp_div(&bi_bit_count, &bi_k, &bi_bit_count, NULL)) != MP_OKAY) {
      *size = 0;
      goto LTM_E1;
   }
   /*
      The first addition of one is done because of the truncating last division,
      and the second addition of one is done to make room for NIL ('\0').
    */
   *size = s_get_int(&bi_bit_count) + 1 + 1 ;

#if ( (defined MP_8BIT) && (INT_MAX > 0xFFFF))
   /* TODO: Add a third table? But how likely is it that "int" is 32-bit in
            an 8-bit environment? */

   /* diff. is 3 bits, hence add 2 (two) */
   *size += 2;
#endif

LTM_E1:
   mp_clear_multi(&bi_bit_count, &bi_k, NULL);
#if ( (defined MP_8BIT) || (defined MP_16BIT) )
   mp_clear(&bi_k_bis);
#endif
   return e;

}
#else
int mp_radix_size(const mp_int *a, int radix, int *size)
{
   int     res, digs;
   mp_int  t;
   mp_digit d;

   *size = 0;

   /* make sure the radix is in range */
   if ((radix < 2) || (radix > 64)) {
      return MP_VAL;
   }

   if (mp_iszero(a) == MP_YES) {
      *size = 2;
      return MP_OKAY;
   }

   /* special case for binary */
   if (radix == 2) {
      *size = mp_count_bits(a) + ((a->sign == MP_NEG) ? 1 : 0) + 1;
      return MP_OKAY;
   }

   /* digs is the digit count */
   digs = 0;

   /* if it's negative add one for the sign */
   if (a->sign == MP_NEG) {
      ++digs;
   }

   /* init a copy of the input */
   if ((res = mp_init_copy(&t, a)) != MP_OKAY) {
      return res;
   }

   /* force temp to positive */
   t.sign = MP_ZPOS;

   /* fetch out all of the digits */
   while (mp_iszero(&t) == MP_NO) {
      if ((res = mp_div_d(&t, (mp_digit)radix, &t, &d)) != MP_OKAY) {
         mp_clear(&t);
         return res;
      }
      ++digs;
   }
   mp_clear(&t);

   /* return digs + 1, the 1 is for the NULL byte that would be required. */
   *size = digs + 1;
   return MP_OKAY;
}


#endif

#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
