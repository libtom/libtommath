CC = gcc
CFLAGS  +=  -Wall -W -Wshadow -ansi -O3 -fomit-frame-pointer -funroll-loops 

VERSION=0.09

default: test

test: bn.o demo.o
	$(CC) bn.o demo.o -o demo
	cd mtest ; gcc $(CFLAGS) mtest.c -o mtest.exe -s

# builds the x86 demo
test86:
	nasm -f coff timer.asm
	$(CC) -DDEBUG -DTIMER_X86 $(CFLAGS) bn.c demo.c timer.o -o demo -s

docdvi: bn.tex
	latex bn
	
docs:	docdvi
	pdflatex bn
	rm -f bn.log bn.aux bn.dvi
	
clean:
	rm -f *.pdf *.o *.exe mtest/*.exe etc/*.exe bn.log bn.aux bn.dvi *.s 

zipup: clean docs
	chdir .. ; rm -rf ltm* libtommath-$(VERSION) ; mkdir libtommath-$(VERSION) ; \
	cp -R ./libtommath/* ./libtommath-$(VERSION)/ ; tar -c libtommath-$(VERSION)/* > ltm-$(VERSION).tar ; \
	bzip2 -9vv ltm-$(VERSION).tar ; zip -9 -r ltm-$(VERSION).zip libtommath-$(VERSION)/*