set term pngcairo size 720,540
# Good for most colorblinds
set colorsequence podo

set key top left;

set ylabel "Time"
set xlabel "Operand size (limbs)"

set output "multiplying".ARG1.".png";
set title "Comparing fast and slow multiplying [".ARG1." bits limbsize]";
plot "multiplying".ARG1."" using 1:2 w lines t "slow", "multiplying".ARG1."" using 1:3 w lines t "fast"

set output "squaring".ARG1.".png";
set title "Comparing fast and slow squaring [".ARG1." bits limbsize]";
plot "squaring".ARG1."" using 1:2 w lines t "slow", "squaring".ARG1."" using 1:3 w lines t "fast"

set xlabel "Operand size (bits)"
set output "readradix".ARG1.".png";
set title "Comparing fast and slow radix conversion (reading) [".ARG1." bits limbsize]";
plot "readradix".ARG1."" using 1:2 w lines t "slow", "readradix".ARG1."" using 1:3 w lines t "fast"

set output "writeradix".ARG1.".png";
set title "Comparing fast and slow radix conversion (writing) [".ARG1." bits limbsize]";
plot "writeradix".ARG1."" using 1:2 w lines t "slow", "writeradix".ARG1."" using 1:3 w lines t "fast"




