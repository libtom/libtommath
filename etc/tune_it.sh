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
# It is not a good idea too e.g: watch high resolution videos while this    #
# test are running if you have not enough memory to avoid page faults.      #
#############################################################################
LIMIT=1000

echo "You might watch the numbers go up to $LIMIT but it will take a long time!"

echo "km ks tc3m tc3s tc4m tc4s tc5m tc5s" > $FILE_NAME
i=1
while [ $i -le $LIMIT ]; do
   RNUM=$(LCG)
   echo $i;
   "$MPWD"/tune -t -L 1 -S "$RNUM" >> $FILE_NAME
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
# rm $FILE_NAME
