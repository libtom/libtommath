nasm -f coff timer.asm
gcc -Wall -W -O3 -fomit-frame-pointer -funroll-loops -DTIMER_X86 demo.c bn.c timer.o -o demo
gcc -I./mtest/ -DU_MPI -Wall -W -O3 -fomit-frame-pointer -funroll-loops -DTIMER_X86 demo.c mtest/mpi.c timer.o -o mpidemo
