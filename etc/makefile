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
        
clean:
	rm -f *.o *.exe pprime tune mersenne 