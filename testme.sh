#!/bin/bash
#
# return values of this script are:
#   0  success
# 128  a test failed
#  >0  the number of timed-out tests

set -e

ret=0
TEST_CFLAGS=""

if [ "$#" == "1" ]
then
  MAKE_OPTIONS=$1
fi

_die()
{
  echo "error $2 while $1"
  if [ "$2" != "124" ]
  then
    exit 128
  else
    echo "assuming timeout while running test - continue"
    ret=$(( $ret + 1 ))
  fi
}

_runtest()
{
  echo -ne " Compile $1 $2"
  make clean > /dev/null
  CC="$1" CFLAGS="$2 $TEST_CFLAGS" make test_standalone $MAKE_OPTIONS > /dev/null 2>test_errors.txt
  echo -e "\rRun test $1 $2"
  timeout --foreground 120 ./test > test_$(echo ${1}${2}  | tr ' ' '_').txt || _die "running tests" $?
}

compilers=( clang gcc )

echo "uname="$(uname -a)

for i in "${compilers[@]}"
do
  if [ -z "$(which $i)" ]
  then
    continue
  fi
  compiler_version=$(echo "$i="$($i -dumpversion))
  if [ "$compiler_version" == "clang=4.2.1" ]
  then
    # one of my versions of clang complains about some stuff in stdio.h and stdarg.h ...
    TEST_CFLAGS="-Wno-typedef-redefinition"
  else
    TEST_CFLAGS=""
  fi
  echo $compiler_version
  _runtest "$i" ""
  _runtest "$i" "-DMP_8BIT"
  _runtest "$i" "-DMP_16BIT"
  _runtest "$i" "-DMP_32BIT"
  _runtest "$i -m32" ""
  _runtest "$i -m32" "-DMP_8BIT"
  _runtest "$i -m32" "-DMP_16BIT"
  _runtest "$i -m32" "-DMP_32BIT"
  if [ "$i" != "clang" ]
  then
    _runtest "$i -mx32" ""
    _runtest "$i -mx32" "-DMP_8BIT"
    _runtest "$i -mx32" "-DMP_16BIT"
    _runtest "$i -mx32" "-DMP_32BIT"
  fi
done

if [ "$ret" == "0" ]
then
  echo "Tests successful"
else
  echo "$ret tests timed out"
fi

exit $ret
