#include <time.h>


#ifdef U_MPI
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
   #include "mpi.h"
   #ifdef _MSC_VER
      typedef __int64            ulong64;
   #else
      typedef unsigned long long ulong64;
   #endif   
#else   
   #include "tommath.h"
#endif

#ifdef TIMER
ulong64 _tt;
void reset(void) { _tt = clock(); }
ulong64 rdtsc(void) { return clock() - _tt; }
#endif

#ifndef DEBUG
int _ifuncs;
#else
extern int _ifuncs;
extern void dump_timings(void);
extern void reset_timings(void);
#endif
   
void ndraw(mp_int *a, char *name)
{
   char buf[4096];
   printf("%s: ", name);
   mp_toradix(a, buf, 10);
   printf("%s\n", buf);
}

static void draw(mp_int *a)
{
   ndraw(a, "");
}


unsigned long lfsr = 0xAAAAAAAAUL;

int lbit(void)
{
   if (lfsr & 0x80000000UL) {
      lfsr = ((lfsr << 1) ^ 0x8000001BUL) & 0xFFFFFFFFUL;
      return 1;
   } else {
      lfsr <<= 1;
      return 0;
   }
}   
     
#ifdef U_MPI
int mp_reduce_setup(mp_int *a, mp_int *b)
{
   int res;
   
   mp_set(a, 1);
   if ((res = s_mp_lshd(a, b->used * 2)) != MP_OKAY) {
      return res;
   }
   return mp_div(a, b, a, NULL);
}

int mp_rand(mp_int *a, int c)
{
   long z = abs(rand()) & 65535;
   mp_set(a, z?z:1);
   while (c--) {
      s_mp_lshd(a, 1);
      mp_add_d(a, abs(rand()), a);
   }
   return MP_OKAY;
}
#endif

   char cmd[4096], buf[4096];
int main(void)
{
   mp_int a, b, c, d, e, f;
   unsigned long expt_n, add_n, sub_n, mul_n, div_n, sqr_n, mul2d_n, div2d_n, gcd_n, lcm_n, inv_n,
                 div2_n, mul2_n;
   unsigned rr;
   int cnt;

#ifdef TIMER
   int n;
   ulong64 tt;
   FILE *log;
#endif

   mp_init(&a);
   mp_init(&b);
   mp_init(&c);
   mp_init(&d);
   mp_init(&e);
   mp_init(&f);

#ifdef TIMER
      printf("CLOCKS_PER_SEC == %lu\n", CLOCKS_PER_SEC);
goto expttime;      

      log = fopen("add.log", "w");
      for (cnt = 4; cnt <= 128; cnt += 4) {
         mp_rand(&a, cnt);
         mp_rand(&b, cnt);
         reset();
         for (rr = 0; rr < 10000000; rr++) {
             mp_add(&a, &b, &c);
         }
         tt = rdtsc();
         printf("Adding\t\t%4d-bit => %9llu/sec, %9llu ticks\n", mp_count_bits(&a), (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt, tt);
         fprintf(log, "%d,%9llu\n", cnt, (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt);
      }
      fclose(log);
 
      log = fopen("sub.log", "w");
      for (cnt = 4; cnt <= 128; cnt += 4) {
         mp_rand(&a, cnt);
         mp_rand(&b, cnt);
         reset();
         for (rr = 0; rr < 10000000; rr++) {
             mp_sub(&a, &b, &c);
         }
         tt = rdtsc();
         printf("Subtracting\t\t%4d-bit => %9llu/sec, %9llu ticks\n", mp_count_bits(&a), (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt, tt);
         fprintf(log, "%d,%9llu\n", cnt, (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt);
      }
      fclose(log);
      
multtime:      

   log = fopen("sqr.log", "w");
   for (cnt = 4; cnt <= 128; cnt += 4) {
      mp_rand(&a, cnt);
      reset();
      for (rr = 0; rr < 250000; rr++) {
          mp_sqr(&a, &b);
      }
      tt = rdtsc();
      printf("Squaring\t%4d-bit => %9llu/sec, %9llu ticks\n", mp_count_bits(&a), (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt, tt);
      fprintf(log, "%d,%9llu\n", cnt, (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt);
   }
   fclose(log);
   
   log = fopen("mult.log", "w");
   for (cnt = 4; cnt <= 128; cnt += 4) {
      mp_rand(&a, cnt);
      mp_rand(&b, cnt);
      reset();
      for (rr = 0; rr < 250000; rr++) {
          mp_mul(&a, &b, &c);
      }
      tt = rdtsc();
      printf("Multiplying\t%4d-bit => %9llu/sec, %9llu ticks\n", mp_count_bits(&a), (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt, tt);
      fprintf(log, "%d,%9llu\n", cnt, (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt);
   }
   fclose(log);

expttime:  
   {
      char *primes[] = {
         "17933601194860113372237070562165128350027320072176844226673287945873370751245439587792371960615073855669274087805055507977323024886880985062002853331424203",
         "2893527720709661239493896562339544088620375736490408468011883030469939904368086092336458298221245707898933583190713188177399401852627749210994595974791782790253946539043962213027074922559572312141181787434278708783207966459019479487",
         "347743159439876626079252796797422223177535447388206607607181663903045907591201940478223621722118173270898487582987137708656414344685816179420855160986340457973820182883508387588163122354089264395604796675278966117567294812714812796820596564876450716066283126720010859041484786529056457896367683122960411136319",
         "47266428956356393164697365098120418976400602706072312735924071745438532218237979333351774907308168340693326687317443721193266215155735814510792148768576498491199122744351399489453533553203833318691678263241941706256996197460424029012419012634671862283532342656309677173602509498417976091509154360039893165037637034737020327399910409885798185771003505320583967737293415979917317338985837385734747478364242020380416892056650841470869294527543597349250299539682430605173321029026555546832473048600327036845781970289288898317888427517364945316709081173840186150794397479045034008257793436817683392375274635794835245695887",
         "436463808505957768574894870394349739623346440601945961161254440072143298152040105676491048248110146278752857839930515766167441407021501229924721335644557342265864606569000117714935185566842453630868849121480179691838399545644365571106757731317371758557990781880691336695584799313313687287468894148823761785582982549586183756806449017542622267874275103877481475534991201849912222670102069951687572917937634467778042874315463238062009202992087620963771759666448266532858079402669920025224220613419441069718482837399612644978839925207109870840278194042158748845445131729137117098529028886770063736487420613144045836803985635654192482395882603511950547826439092832800532152534003936926017612446606135655146445620623395788978726744728503058670046885876251527122350275750995227",
         "11424167473351836398078306042624362277956429440521137061889702611766348760692206243140413411077394583180726863277012016602279290144126785129569474909173584789822341986742719230331946072730319555984484911716797058875905400999504305877245849119687509023232790273637466821052576859232452982061831009770786031785669030271542286603956118755585683996118896215213488875253101894663403069677745948305893849505434201763745232895780711972432011344857521691017896316861403206449421332243658855453435784006517202894181640562433575390821384210960117518650374602256601091379644034244332285065935413233557998331562749140202965844219336298970011513882564935538704289446968322281451907487362046511461221329799897350993370560697505809686438782036235372137015731304779072430260986460269894522159103008260495503005267165927542949439526272736586626709581721032189532726389643625590680105784844246152702670169304203783072275089194754889511973916207",
         "1214855636816562637502584060163403830270705000634713483015101384881871978446801224798536155406895823305035467591632531067547890948695117172076954220727075688048751022421198712032848890056357845974246560748347918630050853933697792254955890439720297560693579400297062396904306270145886830719309296352765295712183040773146419022875165382778007040109957609739589875590885701126197906063620133954893216612678838507540777138437797705602453719559017633986486649523611975865005712371194067612263330335590526176087004421363598470302731349138773205901447704682181517904064735636518462452242791676541725292378925568296858010151852326316777511935037531017413910506921922450666933202278489024521263798482237150056835746454842662048692127173834433089016107854491097456725016327709663199738238442164843147132789153725513257167915555162094970853584447993125488607696008169807374736711297007473812256272245489405898470297178738029484459690836250560495461579533254473316340608217876781986188705928270735695752830825527963838355419762516246028680280988020401914551825487349990306976304093109384451438813251211051597392127491464898797406789175453067960072008590614886532333015881171367104445044718144312416815712216611576221546455968770801413440778423979",
         NULL         
      };
   log = fopen("expt.log", "w");
   for (n = 0; primes[n]; n++) {
      mp_read_radix(&a, primes[n], 10);
      mp_zero(&b);
      for (rr = 0; rr < mp_count_bits(&a); rr++) {
         mp_mul_2(&b, &b);
         b.dp[0] |= lbit();
         b.used  += 1;
      }
      mp_sub_d(&a, 1, &c);
      mp_mod(&b, &c, &b);
      mp_set(&c, 3);
      reset();
      for (rr = 0; rr < 50; rr++) {
          mp_exptmod(&c, &b, &a, &d);
      }
      tt = rdtsc();
      mp_sub_d(&a, 1, &e);
      mp_sub(&e, &b, &b);
      mp_exptmod(&c, &b, &a, &e);  /* c^(p-1-b) mod a */
      mp_mulmod(&e, &d, &a, &d);   /* c^b * c^(p-1-b) == c^p-1 == 1 */
      if (mp_cmp_d(&d, 1)) {
         printf("Different (%d)!!!\n", mp_count_bits(&a));
         draw(&d);
         exit(0);
      }
      printf("Exponentiating\t%4d-bit => %9llu/sec, %9llu ticks\n", mp_count_bits(&a), (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt, tt);
      fprintf(log, "%d,%9llu\n", cnt, (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt);
   }
   }   
   fclose(log);
invtime:
   log = fopen("invmod.log", "w");
   for (cnt = 4; cnt <= 128; cnt += 4) {
      mp_rand(&a, cnt);
      mp_rand(&b, cnt);
      
      do {
         mp_add_d(&b, 1, &b);
         mp_gcd(&a, &b, &c);
      } while (mp_cmp_d(&c, 1) != MP_EQ);
      
      reset();
      for (rr = 0; rr < 10000; rr++) {
          mp_invmod(&b, &a, &c);
      }
      tt = rdtsc();
      mp_mulmod(&b, &c, &a, &d);
      if (mp_cmp_d(&d, 1) != MP_EQ) {
         printf("Failed to invert\n");
         return 0;
      }
      printf("Inverting mod\t%4d-bit => %9llu/sec, %9llu ticks\n", mp_count_bits(&a), (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt, tt);
      fprintf(log, "%d,%9llu\n", cnt, (((unsigned long long)rr)*CLOCKS_PER_SEC)/tt);
   }
   fclose(log);
   
   return 0;
  
#endif

   div2_n = mul2_n = inv_n = expt_n = lcm_n = gcd_n = add_n = 
   sub_n = mul_n = div_n = sqr_n = mul2d_n = div2d_n = cnt = 0;
   for (;;) {
       if (!(++cnt & 15)) sleep(3);
   
       /* randomly clear and re-init one variable, this has the affect of triming the alloc space */
       switch (abs(rand()) % 7) {
           case 0:  mp_clear(&a); mp_init(&a); break;
           case 1:  mp_clear(&b); mp_init(&b); break;
           case 2:  mp_clear(&c); mp_init(&c); break;
           case 3:  mp_clear(&d); mp_init(&d); break;
           case 4:  mp_clear(&e); mp_init(&e); break;
           case 5:  mp_clear(&f); mp_init(&f); break;
           case 6:  break; /* don't clear any */
       }
   
   
       printf("%7lu/%7lu/%7lu/%7lu/%7lu/%7lu/%7lu/%7lu/%7lu/%7lu/%7lu/%7lu/%7lu ", add_n, sub_n, mul_n, div_n, sqr_n, mul2d_n, div2d_n, gcd_n, lcm_n, expt_n, inv_n, div2_n, mul2_n);
       fgets(cmd, 4095, stdin);
       cmd[strlen(cmd)-1] = 0;
       printf("%s  ]\r",cmd); fflush(stdout);
       if (!strcmp(cmd, "mul2d")) { ++mul2d_n; 
          fgets(buf, 4095, stdin); mp_read_radix(&a, buf, 10);
          fgets(buf, 4095, stdin); sscanf(buf, "%d", &rr);
          fgets(buf, 4095, stdin); mp_read_radix(&b, buf, 10);
          
          mp_mul_2d(&a, rr, &a);
          a.sign = b.sign;
          if (mp_cmp(&a, &b) != MP_EQ) {
             printf("mul2d failed, rr == %d\n",rr);
             draw(&a);
             draw(&b);
             return 0;
          }
       } else if (!strcmp(cmd, "div2d")) { ++div2d_n; 
          fgets(buf, 4095, stdin); mp_read_radix(&a, buf, 10);
          fgets(buf, 4095, stdin); sscanf(buf, "%d", &rr);
          fgets(buf, 4095, stdin); mp_read_radix(&b, buf, 10);
          
          mp_div_2d(&a, rr, &a, &e);
          a.sign = b.sign;
          if (a.used == b.used && a.used == 0) { a.sign = b.sign = MP_ZPOS; }
          if (mp_cmp(&a, &b) != MP_EQ) {
             printf("div2d failed, rr == %d\n",rr);
             draw(&a);
             draw(&b);
             return 0;
          }
       } else if (!strcmp(cmd, "add")) { ++add_n;
          fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
          fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
          fgets(buf, 4095, stdin);  mp_read_radix(&c, buf, 10);
          mp_copy(&a, &d);
          mp_add(&d, &b, &d);
          if (mp_cmp(&c, &d) != MP_EQ) {
             printf("add %lu failure!\n", add_n); 
draw(&a);draw(&b);draw(&c);draw(&d);             
             return 0;
          }
          
          /* test the sign/unsigned storage functions */
          
          rr = mp_signed_bin_size(&c);
          mp_to_signed_bin(&c, (unsigned char *)cmd);
          memset(cmd+rr, rand()&255, sizeof(cmd)-rr);
          mp_read_signed_bin(&d, (unsigned char *)cmd, rr);
          if (mp_cmp(&c, &d) != MP_EQ) {
             printf("mp_signed_bin failure!\n");
             draw(&c);
             draw(&d);
             return 0;
          }
                    
          
          rr = mp_unsigned_bin_size(&c);
          mp_to_unsigned_bin(&c, (unsigned char *)cmd);
          memset(cmd+rr, rand()&255, sizeof(cmd)-rr);
          mp_read_unsigned_bin(&d, (unsigned char *)cmd, rr);
          if (mp_cmp_mag(&c, &d) != MP_EQ) {
             printf("mp_unsigned_bin failure!\n");
             draw(&c);
             draw(&d);
             return 0;
          }

       } else if (!strcmp(cmd, "sub")) { ++sub_n;
          fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
          fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
          fgets(buf, 4095, stdin);  mp_read_radix(&c, buf, 10);
          mp_copy(&a, &d);
          mp_sub(&d, &b, &d);
          if (mp_cmp(&c, &d) != MP_EQ) {
             printf("sub %lu failure!\n", sub_n); 
draw(&a);draw(&b);draw(&c);draw(&d);             
             return 0;
          }
       } else if (!strcmp(cmd, "mul")) { ++mul_n;
          fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
          fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
          fgets(buf, 4095, stdin);  mp_read_radix(&c, buf, 10);
          mp_copy(&a, &d);
          mp_mul(&d, &b, &d);
          if (mp_cmp(&c, &d) != MP_EQ) {
             printf("mul %lu failure!\n", mul_n); 
draw(&a);draw(&b);draw(&c);draw(&d);             
             return 0;
          }
       } else if (!strcmp(cmd, "div")) { ++div_n;
          fgets(buf, 4095, stdin); mp_read_radix(&a, buf, 10);
          fgets(buf, 4095, stdin); mp_read_radix(&b, buf, 10);
          fgets(buf, 4095, stdin); mp_read_radix(&c, buf, 10);
          fgets(buf, 4095, stdin); mp_read_radix(&d, buf, 10);
          
          mp_div(&a, &b, &e, &f);
          if (mp_cmp(&c, &e) != MP_EQ || mp_cmp(&d, &f) != MP_EQ) {
             printf("div %lu failure!\n", div_n); 
draw(&a);draw(&b);draw(&c);draw(&d); draw(&e); draw(&f);
             return 0;
          }
          
       } else if (!strcmp(cmd, "sqr")) { ++sqr_n;
          fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
          fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
          mp_copy(&a, &c);
          mp_sqr(&c, &c);
          if (mp_cmp(&b, &c) != MP_EQ) {
             printf("sqr %lu failure!\n", sqr_n); 
draw(&a);draw(&b);draw(&c);
             return 0;
          }
       } else if (!strcmp(cmd, "gcd")) { ++gcd_n;
          fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
          fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
          fgets(buf, 4095, stdin);  mp_read_radix(&c, buf, 10);
          mp_copy(&a, &d);
          mp_gcd(&d, &b, &d);
          d.sign = c.sign;
          if (mp_cmp(&c, &d) != MP_EQ) {
             printf("gcd %lu failure!\n", gcd_n); 
draw(&a);draw(&b);draw(&c);draw(&d);
             return 0;
          }
       } else if (!strcmp(cmd, "lcm")) { ++lcm_n;
             fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
             fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
             fgets(buf, 4095, stdin);  mp_read_radix(&c, buf, 10);
             mp_copy(&a, &d);
             mp_lcm(&d, &b, &d);
             d.sign = c.sign;
             if (mp_cmp(&c, &d) != MP_EQ) {
                printf("lcm %lu failure!\n", lcm_n); 
   draw(&a);draw(&b);draw(&c);draw(&d);
                return 0;
             }
       } else if (!strcmp(cmd, "expt")) {  ++expt_n;
             fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
             fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
             fgets(buf, 4095, stdin);  mp_read_radix(&c, buf, 10);
             fgets(buf, 4095, stdin);  mp_read_radix(&d, buf, 10);
             mp_copy(&a, &e);
             mp_exptmod(&e, &b, &c, &e);
             if (mp_cmp(&d, &e) != MP_EQ) {
                printf("expt %lu failure!\n", expt_n); 
   draw(&a);draw(&b);draw(&c);draw(&d); draw(&e);
                return 0;
             }
       } else if (!strcmp(cmd, "invmod")) {  ++inv_n;
             fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
             fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
             fgets(buf, 4095, stdin);  mp_read_radix(&c, buf, 10);
             mp_invmod(&a, &b, &d);
             mp_mulmod(&d,&a,&b,&e);
             if (mp_cmp_d(&e, 1) != MP_EQ) {
                printf("inv [wrong value from MPI?!] failure\n");
                draw(&a);draw(&b);draw(&c);draw(&d);
                mp_gcd(&a, &b, &e);
                draw(&e);
                return 0;
             }
                
       } else if (!strcmp(cmd, "div2")) { ++div2_n;
             fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
             fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
             mp_div_2(&a, &c);
             if (mp_cmp(&c, &b) != MP_EQ) {
                 printf("div_2 %lu failure\n", div2_n);
                 draw(&a);
                 draw(&b);
                 draw(&c);
                 return 0;
             }
       } else if (!strcmp(cmd, "mul2")) { ++mul2_n;
             fgets(buf, 4095, stdin);  mp_read_radix(&a, buf, 10);
             fgets(buf, 4095, stdin);  mp_read_radix(&b, buf, 10);
             mp_mul_2(&a, &c);
             if (mp_cmp(&c, &b) != MP_EQ) {
                 printf("mul_2 %lu failure\n", mul2_n);
                 draw(&a);
                 draw(&b);
                 draw(&c);
                 return 0;
             }
       }             
       
   }
   return 0;   
}

