CFLAGS += -Wall -W -Wextra -Wshadow -O3 -I../

# default lib name (requires install with root)
# LIBNAME=-ltommath

# libname when you can't install the lib with install
LIBNAME=../libtommath.a

#provable primes
pprime: pprime.o
	$(CC) $(CFLAGS) pprime.o $(LIBNAME) -o pprime

# portable [well requires clock()] tuning app
tune: tune.o statistic_summary_single_column.o
	# The actual benchmark program
	$(CC) $(CFLAGS) tune.o $(LIBNAME) -o tune
	# Computes basic statistics (min, max, mean, median, std. dev., skewness, and kurtosis)
	$(CC) $(CFLAGS) statistic_summary_single_column.o -o statistic_summary_single_column
	# a small script to run 'em
	/bin/sh tune_it.sh

test_standalone: tune.o
	# The benchmark program works as a testtool, too
	$(CC) $(CFLAGS) tune.o $(LIBNAME) -o test
        
# spits out mersenne primes
mersenne: mersenne.o
	$(CC) $(CFLAGS) mersenne.o $(LIBNAME) -o mersenne

# finds DR safe primes for the given config
drprime: drprime.o
	$(CC) $(CFLAGS) drprime.o $(LIBNAME) -o drprime
	
# finds 2k safe primes for the given config
2kprime: 2kprime.o
	$(CC) $(CFLAGS) 2kprime.o $(LIBNAME) -o 2kprime

mont: mont.o
	$(CC) $(CFLAGS) mont.o $(LIBNAME) -o mont

        
clean:
	rm -f *.log *.o *.obj *.exe pprime tune mersenne drprime mont 2kprime pprime.dat \
        statistic_summary_single_column tuning_list multiplying squaring test *.da *.dyn *.dpi *~

