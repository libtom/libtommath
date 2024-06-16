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

# Make pretty pictures (1000 is the maximum number of limbs to print for mul/sqr)
# "tune" runs twice because it runs automatically when build.
graphs: tune
	./tune_it.sh 1000

clean:
	rm -f *.log *.o *.obj *.exe pprime tune mersenne drprime mont 2kprime pprime.dat \
        tuning_list get_limbsize out *.da *.dyn *.dpi *~ cmake_install.cmake Makefile
	rm -rf .libs
	rm -rf CMakeFiles

.PHONY: tune
