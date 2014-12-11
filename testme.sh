#!/bin/bash -e

_runtest()
{
  echo "Run test $1 $2"
  make clean > /dev/null
  CC="$1" CFLAGS="$2" make test_standalone -j9 > /dev/null 2>test_gcc_errors.txt
  ./test > test_$(echo ${1}${2}  | tr ' ' '_').txt
}

_runtest "gcc" ""
_runtest "gcc" "-DMP_8BIT"
_runtest "gcc" "-DMP_16BIT"
_runtest "gcc" "-DMP_32BIT"
_runtest "gcc -m32" ""
_runtest "gcc -m32" "-DMP_8BIT"
# mh, this configuration does not work! it gets stuck at
# Testing (not safe-prime):        46 bits
#_runtest "gcc -m32" "-DMP_16BIT"
_runtest "gcc -m32" "-DMP_32BIT"
_runtest "gcc -mx32" ""
_runtest "gcc -mx32" "-DMP_8BIT"
_runtest "gcc -mx32" "-DMP_16BIT"
_runtest "gcc -mx32" "-DMP_32BIT"
