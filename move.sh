#!/bin/bash
for i in *.c; do git mv $i ${i/bn_/}; done

git mv cutoffs.c mp_cutoffs.c
git mv deprecated.c mp_deprecated.c
git mv prime_tab.c mp_prime_tab.c
sed -E -i "s/CUTOFFS_C/MP_CUTOFFS_C/g" tommath_class.h tommath_superclass.h mp_cutoffs.c
sed -E -i "s/DEPRECATED_C/MP_DEPRECATED_C/g" tommath_class.h tommath_superclass.h mp_deprecated.c
sed -E -i "s/PRIME_TAB_C/MP_PRIME_TAB_C/g" tommath_class.h tommath_superclass.h mp_prime_tab.c

sed -E -i 's/BN_([_A-Z0-9]*_C)/\1/g' *.c */*.c etc/*.c *.h
sed -E -i "s/bn_([_a-z0-9]*\.c)/\1/g" *.c */*.c

sed -E -i "s/'BN_' \. //g" helper.pl
sed -E -i "s/BN_//g" helper.pl
sed -E -i "s/bn\*.c/*mp_*.c/g" helper.pl
sed -E -i "s/bn\*.c/*mp_*.c/g" gen.pl

sed -E -i "s/bn_//g" doc/bn.tex
sed -E -i 's/BN\\_//g' doc/bn.tex

sed -E -i "s/BN_H_/TOMMATH_H_/g" tommath.h
sed -E -i "s/TOMMATH_PRIV_H_/TOMMATH_PRIVATE_H_/g" tommath_private.h
sed -E -i 's/BN_##//' tommath_private.h
sed -E -i "s/BN_MP_DIV_SMALL/MP_DIV_SMALL/g" *.c *.h

sed -E -i 's/LTM_ERR/LBL_ERR/g' *.c */*.c

./helper.pl -u

git rm move.sh
git add .
git commit -m 'Execute move.sh - Rename files from bn_* to match the function names.

* git blame <renamed-file> is not affected
* git log --follow <renamed-file> can be used to show log across renames'

./helper.pl -a
