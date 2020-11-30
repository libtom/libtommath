#Makefile for GCC
#
#Tom St Denis

ifeq ($V,1)
silent=
else
silent=@
endif

#default files to install
ifndef LIBNAME
   LIBNAME=libtommath.a
endif

coverage: LIBNAME:=-Wl,--whole-archive $(LIBNAME)  -Wl,--no-whole-archive

include makefile_include.mk

%.o: %.c $(HEADERS)
ifneq ($V,1)
	@echo "   * ${CC} $@"
endif
	${silent} ${CC} -c ${LTM_CFLAGS} $< -o $@

LCOV_ARGS=--directory .

#START_INS
OBJECTS=mp_2expt.o mp_abs.o mp_add.o mp_add_d.o mp_addmod.o mp_and.o mp_clamp.o mp_clear.o mp_clear_multi.o \
mp_cmp.o mp_cmp_d.o mp_cmp_mag.o mp_cnt_lsb.o mp_complement.o mp_copy.o mp_count_bits.o mp_cutoffs.o \
mp_div.o mp_div_2.o mp_div_2d.o mp_div_d.o mp_dr_is_modulus.o mp_dr_reduce.o mp_dr_setup.o \
mp_error_to_string.o mp_exch.o mp_expt_n.o mp_exptmod.o mp_exteuclid.o mp_fread.o mp_from_sbin.o \
mp_from_ubin.o mp_fwrite.o mp_gcd.o mp_get_double.o mp_get_i32.o mp_get_i64.o mp_get_l.o mp_get_mag_u32.o \
mp_get_mag_u64.o mp_get_mag_ul.o mp_grow.o mp_init.o mp_init_copy.o mp_init_i32.o mp_init_i64.o mp_init_l.o \
mp_init_multi.o mp_init_set.o mp_init_size.o mp_init_u32.o mp_init_u64.o mp_init_ul.o mp_invmod.o \
mp_is_square.o mp_kronecker.o mp_lcm.o mp_log_n.o mp_lshd.o mp_mod.o mp_mod_2d.o \
mp_montgomery_calc_normalization.o mp_montgomery_reduce.o mp_montgomery_setup.o mp_mul.o mp_mul_2.o \
mp_mul_2d.o mp_mul_d.o mp_mulmod.o mp_neg.o mp_or.o mp_pack.o mp_pack_count.o mp_prime_fermat.o \
mp_prime_frobenius_underwood.o mp_prime_is_prime.o mp_prime_miller_rabin.o mp_prime_next_prime.o \
mp_prime_rabin_miller_trials.o mp_prime_rand.o mp_prime_strong_lucas_selfridge.o mp_radix_size.o \
mp_radix_size_overestimate.o mp_rand.o mp_rand_source.o mp_read_radix.o mp_reduce.o mp_reduce_2k.o \
mp_reduce_2k_l.o mp_reduce_2k_setup.o mp_reduce_2k_setup_l.o mp_reduce_is_2k.o mp_reduce_is_2k_l.o \
mp_reduce_setup.o mp_root_n.o mp_rshd.o mp_sbin_size.o mp_set.o mp_set_double.o mp_set_i32.o mp_set_i64.o \
mp_set_l.o mp_set_u32.o mp_set_u64.o mp_set_ul.o mp_shrink.o mp_signed_rsh.o mp_sqrmod.o mp_sqrt.o \
mp_sqrtmod_prime.o mp_sub.o mp_sub_d.o mp_submod.o mp_to_radix.o mp_to_sbin.o mp_to_ubin.o mp_ubin_size.o \
mp_unpack.o mp_xor.o mp_zero.o s_mp_add.o s_mp_copy_digs.o s_mp_div_3.o s_mp_div_recursive.o \
s_mp_div_school.o s_mp_div_small.o s_mp_exptmod.o s_mp_exptmod_fast.o s_mp_get_bit.o s_mp_invmod.o \
s_mp_invmod_odd.o s_mp_log.o s_mp_log_2expt.o s_mp_log_d.o s_mp_montgomery_reduce_comba.o s_mp_mul.o \
s_mp_mul_balance.o s_mp_mul_comba.o s_mp_mul_high.o s_mp_mul_high_comba.o s_mp_mul_karatsuba.o \
s_mp_mul_toom.o s_mp_prime_is_divisible.o s_mp_prime_tab.o s_mp_radix_map.o \
s_mp_radix_size_overestimate.o s_mp_rand_platform.o s_mp_sqr.o s_mp_sqr_comba.o s_mp_sqr_karatsuba.o \
s_mp_sqr_toom.o s_mp_sub.o s_mp_zero_buf.o s_mp_zero_digs.o

#END_INS

$(LIBNAME):  $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

#make a profiled library (takes a while!!!)
#
# This will build the library with profile generation
# then run the test demo and rebuild the library.
#
# So far I've seen improvements in the MP math
profiled:
	make CFLAGS="$(CFLAGS) -fprofile-arcs" timing
	./timing
	rm -f *.a *.o timing
	make CFLAGS="$(CFLAGS) -fbranch-probabilities"

#make a single object profiled library
profiled_single: pre_gen
	$(CC) $(LTM_CFLAGS) -fprofile-arcs -c pre_gen/tommath_amalgam.c -o tommath_amalgam.o
	$(CC) $(LTM_CFLAGS) -DMP_VERSION=\"before\" demo/timing.c tommath_amalgam.o -lgcov -o timing
	./timing
	rm -f *.o timing
	$(CC) $(LTM_CFLAGS) -fbranch-probabilities -c pre_gen/tommath_amalgam.c -o tommath_amalgam.o
	$(AR) $(ARFLAGS) $(LIBNAME) tommath_amalgam.o

install: $(LIBNAME)
	install -d $(DESTDIR)$(LIBPATH)
	install -d $(DESTDIR)$(INCPATH)
	install -m 644 $(LIBNAME) $(DESTDIR)$(LIBPATH)
	install -m 644 $(HEADERS_PUB) $(DESTDIR)$(INCPATH)

uninstall:
	rm $(DESTDIR)$(LIBPATH)/$(LIBNAME)
	rm $(HEADERS_PUB:%=$(DESTDIR)$(INCPATH)/%)

DEMOS=test mtest_opponent

define DEMO_template
$(1): demo/$(1).o demo/shared.o $$(LIBNAME)
	$$(CC) $$(LTM_CFLAGS) $$(LTM_LFLAGS) $$^ -o $$@
endef

$(foreach demo, $(strip $(DEMOS)), $(eval $(call DEMO_template,$(demo))))

.PHONY: mtest
mtest:
	cd mtest ; $(CC) $(LTM_CFLAGS) -O0 mtest.c $(LTM_LFLAGS) -o mtest

timing: demo/timing.c $(LIBNAME)
	$(CC) $(LTM_CFLAGS) $^ $(LTM_LFLAGS) -o timing

tune: $(LIBNAME)
	$(MAKE) -C etc tune CFLAGS="$(LTM_CFLAGS) -I../"
	$(MAKE)

# You have to create a file .coveralls.yml with the content "repo_token: <the token>"
# in the base folder to be able to submit to coveralls
coveralls: lcov
	coveralls-lcov

docs manual:
	$(MAKE) -C doc/ $@ V=$(V)

.PHONY: pre_gen cmp
pre_gen:
	mkdir -p pre_gen
	cat *mp_*.c > pre_gen/tommath_amalgam.c

cmp: profiled_single
	$(CC) $(LTM_CFLAGS) -DMP_VERSION=\"after\" demo/timing.c $(LIBNAME) -lgcov -o timing
	./timing
	$(MAKE) -C logs/ cmp

zipup: clean astyle new_file docs
	@# Update the index, so diff-index won't fail in case the pdf has been created.
	@#   As the pdf creation modifies the tex files, git sometimes detects the
	@#   modified files, but misses that it's put back to its original version.
	@git update-index --refresh
	@git diff-index --quiet HEAD -- || ( echo "FAILURE: uncommited changes or not a git" && exit 1 )
	rm -rf libtommath-$(VERSION) ltm-$(VERSION).*
	@# files/dirs excluded from "git archive" are defined in .gitattributes
	git archive --format=tar --prefix=libtommath-$(VERSION)/ HEAD | tar x
	@echo 'fixme check'
	-@(find libtommath-$(VERSION)/ -type f | xargs grep 'FIXM[E]') && echo '############## BEWARE: the "fixme" marker was found !!! ##############' || true
	mkdir -p libtommath-$(VERSION)/doc
	cp doc/bn.pdf libtommath-$(VERSION)/doc/
	$(MAKE) -C libtommath-$(VERSION)/ pre_gen
	tar -c libtommath-$(VERSION)/ | xz -6e -c - > ltm-$(VERSION).tar.xz
	zip -9rq ltm-$(VERSION).zip libtommath-$(VERSION)
	cp doc/bn.pdf bn-$(VERSION).pdf
	rm -rf libtommath-$(VERSION)
	gpg -b -a ltm-$(VERSION).tar.xz
	gpg -b -a ltm-$(VERSION).zip

new_file:
	perl helper.pl --update-files

perlcritic:
	perlcritic *.pl doc/*.pl

c89:
	@echo "Applying substitutions for c89 compatibility..."
	@if grep mp_bool tommath.h > /dev/null; then echo "Already applied"; exit 1; fi
	@sed -i \
	-e '/#include <stdbool.h>/d' \
	-e 's/#include <stdint.h>/#include "tommath_c89.h"/g' \
	-e 's/#include <inttypes.h>/\/*#include <inttypes.h>*\//g' \
	-e 's/bool/mp_bool/g' \
	-e 's/true/MP_YES/g' \
	-e 's/false/MP_NO/g' \
	-e 's/UINT32_MAX/0xFFFFFFFFu/g' \
	-e 's/UINT64_MAX/(mp_u64)-1/g' \
	-e 's/INT32_MAX/0x7FFFFFFF/g' \
        -e 's/INT32_MIN/(-2147483647-1)/g' \
	-e 's/INT64_MAX/(mp_i64)(((mp_u64)1<<63)-1)/g' \
	-e 's/INT64_MIN/(mp_i64)((mp_u64)1<<63)/g' \
	-e 's/SIZE_MAX/((size_t)-1)/g' \
	-e 's/\(PRI[iux]64\)/MP_\1/g' \
	-e 's/uint\([0-9][0-9]*\)_t/mp_u\1/g' \
	-e 's/int\([0-9][0-9]*\)_t/mp_i\1/g' \
	-e 's/__func__/MP_FUNCTION_NAME/g' \
	*.c tommath.h tommath_private.h demo/*.c demo/*.h etc/*.c

c99:
	@echo "Applying substitutions for c99 compatibility..."
	@if ! grep mp_bool tommath.h > /dev/null; then echo "Already applied"; exit 1; fi
	@sed -i \
	-e 's/#include "tommath_c89.h"/#include <stdint.h>\n#include <stdbool.h>/g' \
	-e 's/\/\*#include <inttypes.h>\*\//#include <inttypes.h>/g' \
	-e 's/mp_bool/bool/g' \
	-e 's/MP_YES/true/g' \
	-e 's/MP_NO/false/g' \
	-e 's/false_/MP_NO_/g' \
	-e 's/0xFFFFFFFFu/UINT32_MAX/g' \
	-e 's/(mp_u64)-1/UINT64_MAX/g' \
        -e 's/(-2147483647-1)/INT32_MIN/g' \
	-e 's/0x7FFFFFFF/INT32_MAX/g' \
	-e 's/(mp_i64)((mp_u64)1<<63)/INT64_MIN/g' \
	-e 's/(mp_i64)(((mp_u64)1<<63)-1)/INT64_MAX/g' \
	-e 's/((size_t)-1)/SIZE_MAX/g' \
	-e 's/MP_\(PRI[iux]64\)/\1/g' \
	-e 's/mp_u\([0-9][0-9]*\)/uint\1_t/g' \
	-e 's/mp_i\([0-9][0-9]*\)/int\1_t/g' \
	-e 's/MP_FUNCTION_NAME/__func__/g' \
	*.c tommath.h tommath_private.h demo/*.c demo/*.h etc/*.c

astyle:
	@echo "   * run astyle on all sources"
	@astyle --options=astylerc --formatted $(OBJECTS:.o=.c) tommath*.h demo/*.c etc/*.c mtest/mtest.c
