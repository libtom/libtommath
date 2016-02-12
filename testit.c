#include "tommath.h"
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include <time.h>

#include <string.h>
#include <math.h>

#include <string.h>

static long get_mp_digits(mp_int * a, int base)
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
    log210 = 3.321928094887362347870319429489390175865;
    break;
  case 16:
    log210 = 4.0;
    break;
  default:
    log210 = log(bits) / log(2.0);
    break;
  }
  return (long) (round(bits / log210));
}

static int mp_print(mp_int * a)
{
  char *str, *s0;
  size_t size;
  int err;

  size = (size_t) (get_mp_digits(a, 10) + 10);
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
  printf("%s\n", str);
  free(str);
  return MP_OKAY;
}



#ifndef LOG_TWO
#   define LOG_TWO (0.69314718055994530941723212145817656808)
#endif
#ifndef LOG_TEN
#   define LOG_TEN (2.3025850929940456840179914546843642076)
#endif
long mp_digits2(mp_int * a, int base)
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
    log210 = bits / (log(base) / log(2.0));
    break;
  }
  return (long) (floor(bits / log210));
}

/*
static int SCHOENHAGE_CONVERSION_CUT = 10;

static mp_int *schoenhagecache2;
static size_t schoenhagecache_len2;

int mp_get_str2(mp_int *a, char *string, int digits){
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
//printf("n = %d\n",n);
  mp_init_set(&v,(mp_digit)(10));
  mp_init_multi(&q,&r,NULL);
  // TODO: 1<<n can overflow, check (but it's good for over 3 billion decimal digits)
  if(schoenhagecache_len == 0){
    schoenhagecache = malloc(32 * sizeof(mp_int));
    if(schoenhagecache == NULL){
       return MP_MEM;
    }
    mp_init_set(&(schoenhagecache[0]),(mp_digit)(10));
    for(i=1;i<n;i++){
       mp_init(&(schoenhagecache[i]));
       mp_sqr(&(schoenhagecache[i-1]),&(schoenhagecache[i]));
    }
    schoenhagecache_len = i;
  }
  if(n >= schoenhagecache_len){
    for(i=schoenhagecache_len;i<=n;i++){
       mp_init(&(schoenhagecache[i]));
       mp_sqr(&(schoenhagecache[i-1]),&(schoenhagecache[i]));
    }
    schoenhagecache_len = i;
  }
  //mp_expt_d(&v,(mp_digit)(1<<n),&v);
  //mp_div(a,&v,&q,&r);
  mp_div(a, &(schoenhagecache[n]) ,&q,&r);
  ed = 1 << n;
  mp_get_str(&q, string, digits - ed);
  mp_get_str(&r, string, ed);
  return MP_OKAY;
}
*/
static int SET_STR_CUTOFF = 20;
static int mp_set_str_intern2(mp_int * a, const char *string, int base)
{
  int len, len_low, len_high;
  char *s_low, *s_high;
  mp_int A, B, m;
  int e = MP_OKAY;

  len = strlen(string);
  len_low = len / 2;
  len_high = len - len_low;

  if (len_low < SET_STR_CUTOFF) {
    if ((e = mp_read_radix(a, string, base)) != MP_OKAY) {
      return e;
    }
    return MP_OKAY;
  }
  if ((e = mp_init_set(&m, base)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_init_multi(&A, &B, NULL)) != MP_OKAY) {
    mp_clear(&m);
    return e;
  }
  // Yes, but do _you_ want to do all of the necessary pointer juggling?
  s_high = malloc(sizeof(char) * (len_high + 1));
  if (s_high == NULL) {
    e = MP_MEM;
    goto _ERR;
  }
  memcpy(s_high, string + len_low, len_high);
  s_high[len_high] = '\0';
  if ((e = mp_set_str_intern2(&A, s_high, base)) != MP_OKAY) {
    free(s_high);
    goto _ERR;
  }
  free(s_high);

  s_low = malloc(sizeof(char) * (len_low + 1));
  if (s_low == NULL) {
    e = MP_MEM;
    goto _ERR;
  }
  memcpy(s_low, string, len_low);
  s_low[len_low] = '\0';
  if ((e = mp_set_str_intern2(&B, s_low, base)) != MP_OKAY) {
    free(s_low);
    goto _ERR;
  }
  free(s_low);
  // TODO: caching?
  if ((e = mp_expt_d(&m, len_high, &m)) != MP_OKAY) {
    goto _ERR;
  }
  if ((e = mp_mul(&B, &m, &B)) != MP_OKAY) {
    goto _ERR;
  }
  if ((e = mp_add(&A, &B, a)) != MP_OKAY) {
    goto _ERR;
  }

_ERR:
  mp_clear_multi(&A, &B, &m, NULL);
  return e;
}

int mp_set_str2(mp_int * a, const char *string, int base)
{
  int sign;
  // TODO: trim string?
  if (*string == '-') {
    string++;
    sign = MP_NEG;
  } else {
    sign = MP_ZPOS;
  }
  if (*string == '+') {
    string++;
  }
  mp_set_str_intern2(a, string, base);
  a->sign = sign;
  return MP_OKAY;
}


int main(int argc, char **argv)
{
  mp_int a, b;
  int d;
  int size;
  char *s, *sold;
  clock_t start, stop, diff;
  int msec;
  char *buf;
  size_t flength;

  FILE *fp;

  mp_init(&a);
  mp_init(&b);
  if (argc <= 1) {
    fprintf(stderr, "needs an argument\n");
    exit(EXIT_FAILURE);
  }

/*
  d = atoi(argv[1]);

  mp_rand(&a,d);
  a.sign = MP_NEG;
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
*/

  fp = fopen(argv[1], "r");
  printf("argv = %s\n", argv[1]);
  fseek(fp, 0, SEEK_END);
  flength = ftell(fp);
  printf("flength = %ld\n", flength);
  fseek(fp, 0, SEEK_SET);

  buf = malloc(flength + 1);
  fread(buf, 1, flength, fp);
  fclose(fp);

  *(buf + flength + 1) = '\0';
//  printf("buf = %s\n",buf);

  start = clock();
  //mp_toradix(&a,s,10);
  mp_read_radix(&a, buf, 10);
  stop = clock();
  diff = stop - start;
  msec = diff * 1000 / CLOCKS_PER_SEC;
  printf("ORIGINAL %d %d\n", msec / 1000, msec % 1000);
  //printf("ORIGINAL %s\n",s);
  //printf("%s - ",s);
  start = clock();
//  mp_get_str(&a,sold,0,10);
  mp_set_str(&b, buf, 10);
  stop = clock();
  diff = stop - start;
  msec = diff * 1000 / CLOCKS_PER_SEC;
  printf("NEW      %d %d\n", msec / 1000, msec % 1000);
  //printf("NEW      %s\n",sold);
  //printf("%s\n",sold);
  mp_sub(&a, &b, &a);
  mp_print(&a);
//mp_print(&b);
//  free(s);
//  free(sold);
  mp_clear(&a);
  mp_clear(&b);

  exit(EXIT_SUCCESS);
}

