global loader          ; entry symbol for ELF

MAGIC_NUMBER equ 0x1BADB002   ; multiboot magic number
FLAGS        equ 0x0          ; multiboot flags
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

extern kernel_main            ; C entry point

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

loader:
    ; set up a simple stack (grows downwards)
    mov esp, 0x00200000       ; stack top at 2 MB

    ; call into C
    call kernel_main

.hang:
    jmp .hang                 ; loop forever