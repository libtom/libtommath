nasm -f elf timer.asm
gcc -Wall -W -O3 -fomit-frame-pointer -funroll-loops -DTIMER_X86 demo.c bn.c timer.o -o ltmdemo