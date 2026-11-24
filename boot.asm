section .multiboot
align 4
    dd 0x1BADB002          ; magic multiboot
    dd 0x00                ; flags
    dd -(0x1BADB002)       ; checksum

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
global _start
extern kernel_main       ; <-- вот эта строка была пропущена

_start:
    cli
    mov esp, stack_top
    call kernel_main
.halt:
    cli
    hlt
    jmp .halt

