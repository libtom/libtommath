CFLAGS += -Wall -W -Wshadow -O3 -fomit-frame-pointer -funroll-loops

pprime: pprime.o
	$(CC) pprime.o -ltommath -o pprime

tune: tune.o
	$(CC) tune.o -ltommath -o tune
        
mersenne: mersenne.o
	$(CC) mersenne.o -ltommath -o mersenne
        
clean:
	rm -f *.o pprime tune mersenne


