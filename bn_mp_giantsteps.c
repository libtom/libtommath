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

*/
int mp_giantsteps(int start, int end, int stepsize, int **precs, int *steps)
{
    int i, t;

    if (start < 0 || end <= 0 || stepsize < 2 || end <= start) {
	return MP_VAL;
    }

    t = end;
    i = 1;

    // we need to run this loop twice but it is cheap and runs only
    // log_2(end - start) times max.
    while (1) {
	if (t < start * stepsize) {
	    break;
	}
	i++;
	t = (t / stepsize) + 2;
    }

    *precs = malloc(i * sizeof(int));
    if (*precs == NULL) {
	return MP_MEM;
    }
    *precs += i - 1;
    **precs = end;
    *steps  = i;
    while (1) {
	if (**precs <= start * stepsize) {
	    break;
	}
	(*precs)--;
	**precs = (*((*precs)+1) / stepsize) + 2;
    }
    return MP_OKAY;
}

#endif

