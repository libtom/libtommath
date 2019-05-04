#!/bin/sh

# A linear congruential generator is sufficient for the purpose.
SEED=3735928559
LCG() {
  SEED=$(((1103515245 * $SEED + 12345) % 2147483648))
  echo $SEED
}

MPWD=$(pwd)
FILE_NAME="tuning_list"
BNCORE_C="../bncore.c"
BACKUP_SUFFIX=".orig"
RNUM=0;
#############################################################################
# It would be a good idea to isolate these processes (with e.g.: cpuset)    #
#                                                                           #
# It is not a good idea to e.g: watch high resolution videos while this     #
# test are running if you do not have enough memory to avoid page faults.   #
#############################################################################

# Number of rounds overall. 100 is a relatively small number, 1,000 would be
# better but also slower, of course
LIMIT=100
# Number of loops for each input. 64 is a relatively large number, 10 is also OK
# if LIMIT is big enough (>=1,000)
RLOOPS=64
# Offset (> 0) . Runs tests with asymmetric input of the form 1:OFFSET
OFFSET=1
# Number of positive results (TC-is-faster) accumulated until it is accepted
# (mainly for "noisy" environments, especially if isolation is not possible)
LAG=1
# Keep the temporary file $FILE_NAME. Set to 0 (zero) to remove it at the end.
KEEP_TEMP=1

echo "You might like to watch the numbers go up to $LIMIT but it will take a long time!"

echo "km ks tc3m tc3s tc4m tc4s tc5m tc5s" > $FILE_NAME
i=1
while [ $i -le $LIMIT ]; do
   RNUM=$(LCG)
   echo $i
   "$MPWD"/tune -t -F 0 -r $RLOOPS -L $LAG -S "$RNUM" -o $OFFSET >> $FILE_NAME
   i=$((i + 1))
done

i=1
SUMMARY=""
TMP=""
while [ $i -le 8 ]; do
   if [ $i -eq 1 ]; then
      v="v"
   else
      v=""
   fi
   TMP=$(cat  $FILE_NAME | cut -d' ' -f$i | "$MPWD"/statistic_summary_single_column $v;echo z)
   SUMMARY="$SUMMARY${TMP%z}"
   i=$((i + 1))
done
printf '%s\n' "$SUMMARY"
if [ $KEEP_TEMP -eq 0 ]; then
   rm -v $FILE_NAME
fi

echo "Writing cut-off values to \"bncore.c\" using the median."
echo "In case of failure: a copy of \"bncore.c\" is in \"bncore.c.orig\""
cp -v $BNCORE_C $BNCORE_C$BACKUP_SUFFIX
cat << END_OF_INPUT > $BNCORE_C
#include "tommath_private.h"
#ifdef BNCORE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Known optimal configurations

 CPU                    /Compiler     /MUL CUTOFF/SQR CUTOFF
-------------------------------------------------------------
 Intel P4 Northwood     /GCC v3.4.1   /        88/       128/LTM 0.32 ;-)
 AMD Athlon64           /GCC v3.4.4   /        80/       120/LTM 0.35

*/

/*
   Values evaluated on an AMD A8-6600K (64-bit).
   Please see etc/tune.c for ways to optimize them for your machine but
   be aware that it may take a long time.
 */
END_OF_INPUT

i=1;
TMP=$(cat $FILE_NAME | cut -d' ' -f$i | "$MPWD"/statistic_summary_single_column m;)
echo "int KARATSUBA_MUL_CUTOFF = $TMP;" >> $BNCORE_C
i=$((i + 1))
TMP=$(cat $FILE_NAME | cut -d' ' -f$i | "$MPWD"/statistic_summary_single_column m;)
echo "int KARATSUBA_SQR_CUTOFF = $TMP;" >> $BNCORE_C
i=$((i + 1))
TMP=$(cat $FILE_NAME | cut -d' ' -f$i | "$MPWD"/statistic_summary_single_column m;)
echo "int TOOM_MUL_CUTOFF = $TMP;" >> $BNCORE_C
i=$((i + 1))
TMP=$(cat $FILE_NAME | cut -d' ' -f$i | "$MPWD"/statistic_summary_single_column m;)
echo "int TOOM_SQR_CUTOFF = $TMP;" >> $BNCORE_C
i=$((i + 1))
TMP=$(cat $FILE_NAME | cut -d' ' -f$i | "$MPWD"/statistic_summary_single_column m;)
echo "int TOOM_COOK_4_MUL_CO = $TMP;" >> $BNCORE_C
i=$((i + 1))
TMP=$(cat $FILE_NAME | cut -d' ' -f$i | "$MPWD"/statistic_summary_single_column m;)
echo "int TOOM_COOK_4_SQR_CO = $TMP;" >> $BNCORE_C
i=$((i + 1))
TMP=$(cat $FILE_NAME | cut -d' ' -f$i | "$MPWD"/statistic_summary_single_column m;)
echo "int TOOM_COOK_5_MUL_CO = $TMP;" >> $BNCORE_C
i=$((i + 1))
TMP=$(cat $FILE_NAME | cut -d' ' -f$i | "$MPWD"/statistic_summary_single_column m;)
echo "int TOOM_COOK_5_SQR_CO = $TMP;" >> $BNCORE_C
i=$((i + 1))

cat << END_OF_INPUT >> $BNCORE_C
/* These are just some safe values, please read the documentation for how to get your best fit */
#if (MP_DIGIT_BIT == 60)
int FFT_MUL_LOWER_CO = 75000;
int FFT_MUL_UPPER_CO = 400000;

int FFT_SQR_LOWER_CO = 35000;
int FFT_SQR_UPPER_CO = 400000;
#elif (MP_DIGIT_BIT == 28)
#ifdef MP_FFT_QUARTER_28
int FFT_MUL_LOWER_CO = 85000;
int FFT_MUL_UPPER_CO = 10000000; /* actual upper limit is larger than that */

int FFT_SQR_LOWER_CO = 21000;
int FFT_SQR_UPPER_CO = 10000000; /* actual upper limit is larger than that */
#else
int FFT_MUL_LOWER_CO = 2800;
int FFT_MUL_UPPER_CO = 170000;

int FFT_SQR_LOWER_CO = 4500;
int FFT_SQR_UPPER_CO = 170000;
#endif /* MP_FFT_QUARTER_28 */
#elif (MP_DIGIT_BIT == 15)
int FFT_MUL_LOWER_CO = 27000;
int FFT_MUL_UPPER_CO = 10000000; /* actual upper limit is larger than that */

int FFT_SQR_LOWER_CO = 27000;
int FFT_SQR_UPPER_CO = 10000000; /* actual upper limit is larger than that */
#elif (MP_DIGIT_BIT == 7)
int FFT_MUL_LOWER_CO = 300;
int FFT_MUL_UPPER_CO = 900000;

int FFT_SQR_LOWER_CO = 100;
int FFT_SQR_UPPER_CO = 900000;
#else
#  ifdef _MSC_VER
#    pragma message("7, 15, 28 and 60 bit limbs only (no 31 bit, sorry).")
#  else
#    warning "7, 15, 28 and 60 bit limbs only (no 31 bit, sorry)."
#  endif
#endif /* FFT */
#endif
END_OF_INPUT









