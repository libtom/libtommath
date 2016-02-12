#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#include <unistd.h>

#include <tommath.h>
// may need -lrt
#include <time.h>
#include <math.h>











    #include <sys/time.h>


/*

./subquadratic 4400 2200
0s 32498562
0s 110819856

./subquadratic 4500 2250  
0s 106189618
0s 129122790

./subquadratic 4700 2350
0s 33877377
0s 128771167
diff 0



*/
int main(int argc, char **argv)
{
    int i;
    int start, end, stepsize, *precs, steps, *s, base, size;
    mp_int tmp,t1,t2,t3,t4,q,qq,rr,r;
    struct timeval time1 ,time2, time3, time4;

    mp_init_multi(&tmp,&t1,&t2,&t3,&t4,&q,&qq,&rr,&r,NULL);

    if (argc < 2) {
	printf("Usage: vid. source\n");
	exit(EXIT_FAILURE);
    }
    if(argc >= 2){

//start = atoi(argv[1]); end = atoi(argv[2]); stepsize = atoi(argv[3]);

mp_rand (&t1,atoi(argv[1]));
//mp_rand (&t2,( atoi(argv[2]) ));
mp_copy(&t1,&t3);//mp_copy(&t2,&t4);
   //    mp_read_radix (&t1, argv[1], 10);
       base = atoi(argv[2]);
       mp_read_radix (&t2, argv[2], 10);
    }

/*
 mp_giantsteps(start, end, stepsize, &precs, &steps);

printf("steps = %d\n", steps);
s = precs;
for(i = 0;i<steps;i++){
   printf("%d, ", *precs);
   precs++;
}
puts("");
free(s);
*/
   // printf("t1  = ");mp_put(&t1,10);puts("");
   // printf("t2  = ");mp_put(&t2,10);puts("");
gettimeofday(&time1,NULL);
for(i = 0;i<100;i++){
    mp_ilogb(&t1,&t2,&q);
    //mp_radix_size(&t3,base,&size);
}
gettimeofday(&time2,NULL);

printf("1 %lus %lu\n",(unsigned long)time2.tv_sec - time1.tv_sec,(unsigned long)time2.tv_usec - time1.tv_usec );



//   printf("t1 ");mp_put(&t1,10);puts("");
  // printf("t2 ");mp_put(&t2,10);puts("");
printf("base %d\n",base);
gettimeofday(&time3,NULL);
for(i = 0;i<100;i++){
    mp_radix_size(&t3,base,&size);
    //mp_ilogb(&t1,&t2,&q);
}
gettimeofday(&time4,NULL);

printf("2 %lus %lu\n",(unsigned long)time4.tv_sec - time3.tv_sec,(unsigned long)time4.tv_usec - time3.tv_usec );

    //printf("q new ");mp_put(&q,10);puts("");
    //printf("q old ");mp_put(&qq,10);puts("");

    //printf("r new ");mp_put(&r,10);puts("");
    //printf("r old ");mp_put(&rr,10);puts("");

    //mp_mul(&q,&t2,&t2);
    //mp_add(&t2,&r,&t2);
    //printf("t1 ");mp_put(&t1,10);puts("");
    //printf("t2 ");mp_put(&t2,10);puts("");

    //mp_sub(&t2,&t1,&t2);

    printf("log  = ");mp_put(&q,10);puts("");
    printf("size = %d\n",size);
    //printf("t4 ");mp_put(&t4,10);puts("");
//printf("%u %u\n",strlen(argv[1]),strlen(argv[2]));

    mp_clear_multi(&tmp,&t1,&t2,&t3,&q,&qq,&rr,&r,NULL);

    exit(EXIT_SUCCESS);
}
