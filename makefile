CC = gcc
CFLAGS  += -Wall -W -O3 -funroll-loops

VERSION=0.02

default: test

test: bn.o demo.o
	$(CC) bn.o demo.o -o demo

docdvi: bn.tex
	latex bn
	
docs:	docdvi
	pdflatex bn
	rm -f bn.log bn.aux bn.dvi
	
clean:
	rm -f *.o *.exe mtest/*.exe bn.log bn.aux bn.dvi *.s 

zipup: clean docs
	chdir .. ; rm -rf ltm* libtommath-$(VERSION) ; mkdir libtommath-$(VERSION) ; \
	cp -R ./libtommath/* ./libtommath-$(VERSION)/ ; tar -c libtommath-$(VERSION)/* > ltm-$(VERSION).tar ; \
	bzip2 -9vv ltm-$(VERSION).tar ; zip -9 -r ltm-$(VERSION).zip libtommath-$(VERSION)/*