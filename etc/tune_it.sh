#!/bin/sh

# A linear congruential generator is sufficient for the purpose.
SEED=3735928559
LCG() {
  SEED=$(((1103515245 * $SEED + 12345) % 2147483648))
  echo $SEED
}

MPWD=$(pwd)
FILE_NAME="tuning_list"
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

