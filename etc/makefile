CFLAGS += -Wall -W -Wshadow -O3 -fomit-frame-pointer -funroll-loops -I../


# default lib name (requires install with root)
# LIBNAME=-ltommath

# libname when you can't install the lib with install
LIBNAME=../libtommath.a

pprime: pprime.o
	$(CC) pprime.o $(LIBNAME) -o pprime

tune: tune.o
	$(CC) tune.o $(LIBNAME) -o tune
        
mersenne: mersenne.o
	$(CC) mersenne.o $(LIBNAME) -o mersenne

drprime: drprime.o
	$(CC) drprime.o $(LIBNAME) -o drprime
        
clean:
	rm -f *.log *.o *.obj *.exe pprime tune mersenne drprime