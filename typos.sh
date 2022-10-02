#!/bin/sh

sed -i "s/Dimminished/Diminished/g" logs/before_after.dem
sed -i "s/Dimminished/Diminished/g" logs/graphs.dem
sed -i "s/accomodate/accommodate/g" mp_mul_2.c
sed -i "s/accomodate/accommodate/g" mp_2expt.c
sed -i "s/addtional/additional/g" mp_prime_is_prime.c
sed -i "s/affect/effect/g" demo/mtest_opponent.c
sed -i "s/alot/a lot/g" changes.txt
sed -i "s/alot/a lot/g" mp_exptmod.c
sed -i "s/alot/a lot/g" mp_montgomery_calc_normalization.c
sed -i "s/annonying/annoying/g" astylerc
sed -i "s/asymptopically/asymptotically/g" s_mp_mul_karatsuba.c
sed -i "s/calulates/calculates/g" mp_reduce_setup.c
sed -i "s/configuraion/configuration/g" doc/makefile
sed -i "s/eliminiation/elimination/g" CMakeLists.txt
sed -i "s/embeded/embedded/g" mp_prime_is_prime.c
sed -i "s/endianess/endianness/g" demo/test.c
sed -i "s/iterrate/iterate/g" s_mp_sqr_comba.c
sed -i "s/iterrate/iterate/g" s_mp_mul_high_comba.c
sed -i "s/iterrate/iterate/g" s_mp_mul_comba.c
sed -i "s/maginitude/magnitude/g" mp_cmp_mag.c
sed -i "s/offseting/offsetting/g" s_mp_montgomery_reduce_comba.c
sed -i "s/ofthe/of the/g" doc/bn.tex
sed -i "s/otherway/other way/g" mp_lshd.c
sed -i "s/packagage/package/g" changes.txt
sed -i "s/preemptivly/preemptively/g" mp_prime_is_prime.c
sed -i "s/probabilty/probability/g" doc/bn.tex
sed -i "s/programms/programs/g" doc/bn.tex
sed -i "s/seperate/separate/g" changes.txt
sed -i "s/simplifiy/simplify/g" demo/test.c
sed -i "s/tollchocked/tolchocked/g" mtest/mpi.c
sed -i "s/trimed/trimmed/g" mp_clamp.c
sed -i "s/triming/trimming/g" demo/mtest_opponent.c
sed -i "s/uncommited/uncommitted/g" makefile
sed -i "s/unsused/unused/g" changes.txt
