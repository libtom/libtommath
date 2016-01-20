#include <tommath.h>
#ifdef BN_MP_GIANTSTEPS_C
/*
    Function quite alike the similarily named one from Python.

    Example (error-handling ommitted):

    int start, end, stepsize, *precs, steps, *s;
        ...
    mp_giantsteps(start, end, stepsize, &precs, &steps);

    printf("steps = %d\n", steps);
    s = precs;
    for(i = 0;i<steps;i++){
        printf("%d, ", *precs);
        precs++;
    }
    puts("");
    free(s);

    CAVEAT: the maximum value is listed twice! This has the reason that
            some algorithms (e.g.: integer division with Newton-Raphson)
            need a last round with full precision. It won't hurt the
            result if ignored when not needed it will just add some extra
            run-time.
*/
int mp_giantsteps(int start, int end, int stepsize, int **precs, int *steps)
{
   int i, t;

   if (start < 0 || end <= 0 || stepsize < 2 || end <= start) {
      return MP_VAL;
   }

   t = end;
   i = 0;

   // we need to run this loop twice but it is cheap and runs only
   // log_2(end - start) times max.
   while (1) {
      if (t < start * stepsize) {
         break;
      }
      i++;
      t = (t / stepsize) ;
   }

   *precs = malloc((i+3) * sizeof(int));
   if (*precs == NULL) {
      return MP_MEM;
   }
   // i--;
   *precs += i ;
   **precs = end;
i--;(*precs)--;**precs = end;
   *steps  = 2;
   while (i--) {
      (*precs)--;
      (*steps)++;
      **precs = (*((*precs)+1) / stepsize) ;
   }
   return MP_OKAY;
}

#endif

