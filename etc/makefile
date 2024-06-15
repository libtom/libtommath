LTM_TUNE_CFLAGS = $(CFLAGS) $(LTM_CFLAGS) -Wall -W -Wextra -Wshadow -O3 -I../

# default lib name (requires install with root)
# LIBNAME=-ltommath

# libname when you can't install the lib with install
LIBNAME=../libtommath.a

all: pprime tune test_standalone mersenne drprime 2kprime mont getlimbsize graph

#provable primes
pprime: pprime.o
	$(CC) $(LTM_TUNE_CFLAGS) pprime.o $(LIBNAME) -o pprime

# portable [well requires clock()] tuning app
tune: tune.o
	$(CC) $(LTM_TUNE_CFLAGS) tune.o $(LIBNAME) -o tune
	./tune_it.sh

test_standalone: tune.o
	# The benchmark program works as a testtool, too
	$(CC) $(LTM_TUNE_CFLAGS) tune.o $(LIBNAME) -o test

# spits out mersenne primes
mersenne: mersenne.o
	$(CC) $(LTM_TUNE_CFLAGS) mersenne.o $(LIBNAME) -o mersenne

# finds DR safe primes for the given config
drprime: drprime.o
	$(CC) $(LTM_TUNE_CFLAGS) drprime.o $(LIBNAME) -o drprime

# finds 2k safe primes for the given config
2kprime: 2kprime.o
	$(CC) $(LTM_TUNE_CFLAGS) 2kprime.o $(LIBNAME) -o 2kprime

mont: mont.o
	$(CC) $(LTM_TUNE_CFLAGS) mont.o $(LIBNAME) -o mont

# Reads MP_DIGIT_BIT from tommath.h, so take care that there is the correct MP_xxBIT
# in CFLAGS and/or LTM_CFLAGS when compiling from this directory ("libtommath/etc").
getlimbsize: get_limbsize.o
	$(CC) $(LTM_TUNE_CFLAGS) get_limbsize.o $(LIBNAME) -o get_limbsize

# Make pretty pictures (3000 is the maximum number of limbs to print for mul/sqr)
# "tune" runs twice because it runs automatically when build.
graphs: tune get_limbsize
	./tune_it.sh 3000
	gnuplot -c plot_graphs.gp `./get_limbsize`

clean:
	rm -f *.log *.o *.obj *.exe pprime tune mersenne drprime mont 2kprime pprime.dat \
        tuning_list get_limbsize *.da *.dyn *.dpi *~
	rm -rf .libs

.PHONY: tune
