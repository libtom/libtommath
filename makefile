CFLAGS  +=  -I./ -Wall -W -Wshadow -O3 -fomit-frame-pointer -funroll-loops

VERSION=0.12

default: libtommath.a

#default files to install
LIBNAME=libtommath.a
HEADERS=tommath.h

#LIBPATH-The directory for libtomcrypt to be installed to.
#INCPATH-The directory to install the header files for libtommath.
#DATAPATH-The directory to install the pdf docs.
DESTDIR=
LIBPATH=/usr/lib
INCPATH=/usr/include
DATAPATH=/usr/share/doc/libtommath/pdf

OBJECTS=bncore.o bn_mp_init.o bn_mp_clear.o bn_mp_exch.o bn_mp_grow.o bn_mp_shrink.o \
bn_mp_clamp.o bn_mp_zero.o  bn_mp_set.o bn_mp_set_int.o bn_mp_init_size.o bn_mp_copy.o \
bn_mp_init_copy.o bn_mp_abs.o bn_mp_neg.o bn_mp_cmp_mag.o bn_mp_cmp.o bn_mp_cmp_d.o \
bn_mp_rshd.o bn_mp_lshd.o bn_mp_mod_2d.o bn_mp_div_2d.o bn_mp_mul_2d.o bn_mp_div_2.o \
bn_mp_mul_2.o bn_s_mp_add.o bn_s_mp_sub.o bn_fast_s_mp_mul_digs.o bn_s_mp_mul_digs.o \
bn_fast_s_mp_mul_high_digs.o bn_s_mp_mul_high_digs.o bn_fast_s_mp_sqr.o bn_s_mp_sqr.o \
bn_mp_add.o bn_mp_sub.o bn_mp_karatsuba_mul.o bn_mp_mul.o bn_mp_karatsuba_sqr.o \
bn_mp_sqr.o bn_mp_div.o bn_mp_mod.o bn_mp_add_d.o bn_mp_sub_d.o bn_mp_mul_d.o \
bn_mp_div_d.o bn_mp_mod_d.o bn_mp_expt_d.o bn_mp_addmod.o bn_mp_submod.o \
bn_mp_mulmod.o bn_mp_sqrmod.o bn_mp_gcd.o bn_mp_lcm.o bn_fast_mp_invmod.o bn_mp_invmod.o \
bn_mp_reduce.o bn_mp_montgomery_setup.o bn_fast_mp_montgomery_reduce.o bn_mp_montgomery_reduce.o \
bn_mp_exptmod_fast.o bn_mp_exptmod.o bn_mp_2expt.o bn_mp_n_root.o bn_mp_jacobi.o bn_reverse.o \
bn_mp_count_bits.o bn_mp_read_unsigned_bin.o bn_mp_read_signed_bin.o bn_mp_to_unsigned_bin.o \
bn_mp_to_signed_bin.o bn_mp_unsigned_bin_size.o bn_mp_signed_bin_size.o bn_radix.o \
bn_mp_xor.o bn_mp_and.o bn_mp_or.o bn_mp_rand.o

libtommath.a:  $(OBJECTS)
	$(AR) $(ARFLAGS) libtommath.a $(OBJECTS)
	ranlib libtommath.a

install: libtommath.a docs
	install -d -g root -o root $(DESTDIR)$(LIBPATH)
	install -d -g root -o root $(DESTDIR)$(INCPATH)
	install -d -g root -o root $(DESTDIR)$(DATAPATH)
	install -g root -o root $(LIBNAME) $(DESTDIR)$(LIBPATH)
	install -g root -o root $(HEADERS) $(DESTDIR)$(INCPATH)
	install -g root -o root bn.pdf $(DESTDIR)$(DATAPATH)

test: libtommath.a demo/demo.o
	$(CC) demo/demo.o libtommath.a -o test
	cd mtest ; gcc $(CFLAGS) mtest.c -o mtest -s
        
timing: libtommath.a
	$(CC) $(CFLAGS) -DTIMER demo/demo.c libtommath.a -o ltmtest -s
	$(CC) $(CFLAGS) -DTIMER -DU_MPI -I./mtest/ demo/demo.c mtest/mpi.c -o mpitest -s

docdvi: bn.tex
	latex bn
	
docs:	docdvi
	pdflatex bn
	rm -f bn.log bn.aux bn.dvi
	
clean:
	rm -f *.pdf *.o *.a etclib/*.o demo/demo.o test ltmtest mpitest mtest/mtest \
        bn.log bn.aux bn.dvi *.log *.s
	cd etc ; make clean

zipup: clean docs
	cd .. ; rm -rf ltm* libtommath-$(VERSION) ; mkdir libtommath-$(VERSION) ; \
	cp -R ./libtommath/* ./libtommath-$(VERSION)/ ; tar -c libtommath-$(VERSION)/* > ltm-$(VERSION).tar ; \
	bzip2 -9vv ltm-$(VERSION).tar ; zip -9 -r ltm-$(VERSION).zip libtommath-$(VERSION)/*
