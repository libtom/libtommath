#include "tommath.h"
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include <time.h>

#include <string.h>
#include <math.h>

#ifndef LOG_TWO
#define LOG_TWO (0.69314718055994530941723212145817656808)
#endif
#ifndef LOG_TEN
#define LOG_TEN (2.3025850929940456840179914546843642076)
#endif
long mp_digits(mp_int * a, int base)
{
    double log210;
    long bits = mp_count_bits(a);
    switch (base) {
    case 2:
        log210 = 1.0;
        break;
    case 8:
        log210 = 3.0;
        break;
    case 10:
        log210 = 3.3219280948873623478703194294893901759;
        break;
    case 16:
        log210 = 4.0;
        break;
    default:
        log210 = bits / (log(base)/log(2.0));
        break;
    }
    return (long) (floor(bits / log210));
}

static int SCHOENHAGE_CONVERSION_CUT = 10;

static mp_int *schoenhagecache;
static size_t schoenhagecache_len;

int mp_get_str(mp_int *a, char *string, int digits){
  char *str, *s0;
  int b,n;
  int ed;
  int err,i;
  size_t size;
  mp_int v,q,r;
  if(a->used <= SCHOENHAGE_CONVERSION_CUT){
    size = (size_t) (mp_digits(a, 10) + 10);
    str = malloc(size * sizeof(char));
    if (NULL == str) {
	fprintf(stderr, "malloc failed to allocate %lu bytes\n",
		size * sizeof(char));
	return MP_MEM;
    }
    s0 = str;
    while (--size) {
	*s0++ = '\0';
    }
    if ((err = mp_toradix(a, str, 10)) != MP_OKAY) {
	return err;
    }
    if( (strlen(str) < (unsigned)digits) && (strlen(string) > 0 )){
       for (i = strlen(str); i < digits; i++) { 
          string = strncat(string, "0", 1);
       }
    }
    string = strcat(string, str);
    return MP_OKAY;
  }

  b = mp_count_bits(a);
  n = (int) round(log(b * LOG_TWO / LOG_TEN) / LOG_TWO - 1.0);

  // TODO: restrict to first n
  if(schoenhagecache_len == 0){
    //schoenhagecache = malloc(((sizeof(int)*CHAR_BIT) + 1) * sizeof(mp_int));
    schoenhagecache = malloc(30 * sizeof(mp_int));
    mp_init_set(&v,(mp_digit)(10));
    if(schoenhagecache == NULL){
       return MP_MEM;
    }
    //mp_init(&(schoenhagecache[0]));
    //mp_expt_d(&v,(mp_digit)(1<<1),&(schoenhagecache[0]));
    //for(i=0;i<(int)(sizeof(int)*CHAR_BIT);i++){
    for(i=0;i<21;i++){
       mp_init(&(schoenhagecache[i]));
       mp_expt_d(&v,(mp_digit)(1<<i),&(schoenhagecache[i]));
    }
    schoenhagecache_len = (sizeof(int)*CHAR_BIT);
  }
printf("here n= %d\n",n);
  mp_div(a,&(schoenhagecache[n]),&q,&r);

  mp_init_multi(&q,&r,NULL);
  //mp_expt_d(&v,(mp_digit)(1<<n),&v);
  //mp_div(a,&v,&q,&r);
  // TODO: 1<<n can overflow, check (but it's good for over 3 billion decimal digits)
  ed = 1 << n;
  mp_get_str(&q, string, digits - ed);
  mp_get_str(&r, string, ed);
  return MP_OKAY;
}

int main(int argc, char **argv){
  mp_int a;
  int d;
  int size;
  char *s,*sold;
  clock_t start, stop, diff;
  int msec;

  mp_init(&a);
  if(argc <= 1){
    fprintf(stderr,"needs an argument\n");
    exit(EXIT_FAILURE);
  }
  d = atoi(argv[1]);

  mp_rand(&a,d);
printf("bits = %d\n",mp_count_bits(&a));
  size = round(mp_count_bits(&a)/3.3219280948873623478703194294893901759) + 10;
printf("size = %d\n",size);
  s = malloc(size * sizeof(char));
  if (NULL == s) {
       fprintf(stderr, "malloc failed to allocate %lu bytes\n",
		size * sizeof(char));
	return MP_MEM;
  }
  sold = malloc(size * sizeof(char));
  if (NULL == sold) {
       fprintf(stderr, "malloc failed to allocate %lu bytes\n",
		size * sizeof(char));
	return MP_MEM;
  }

  start = clock();
  mp_toradix(&a,s,10);
  stop = clock();
  diff = stop - start;
  msec = diff * 1000 / CLOCKS_PER_SEC;
  //printf("ORIGINAL %d %d\n",msec/1000, msec%1000);
  //printf("ORIGINAL %s\n",s);
  printf("%s - ",s);
  start = clock();
  mp_get_str(&a,sold,0);
  stop = clock();
  diff = stop - start;
  msec = diff * 1000 / CLOCKS_PER_SEC;
  //printf("NEW      %d %d\n",msec/1000, msec%1000);
  //printf("NEW      %s\n",sold);
  printf("%s\n",sold);
  free(s);
  free(sold);
  mp_clear(&a);
  exit(EXIT_SUCCESS);
}
