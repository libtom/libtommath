; Simple RDTSC reader for NASM
;
; build with "nasm -f ___ timer.asm" where ___ is coff or elf [or whatever]
;
; Most *nix installs use elf so it would be "nasm -f elf timer.asm"
;
; Tom St Denis
[bits 32]
[section .data]
timer dd 0, 0
[section .text]

[global _gettsc]
_gettsc:
   rdtsc
   ret

[global _rdtsc]
_rdtsc:
   rdtsc
   sub eax,[timer]
   sbb edx,[timer+4]
   ret

[global _reset]
_reset:
   push eax
   push edx
   rdtsc
   mov [timer],eax 
   mov [timer+4],edx
   pop edx
   pop eax
   ret
