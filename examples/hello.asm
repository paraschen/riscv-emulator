# Simple RISC-V assembly example
.section .text
.global _start

_start:
    # Write "Hello, RISC-V!" to stdout (UART)
    li a0, 1           # file descriptor (stdout)
    la a1, message     # buffer
    li a2, 14          # length
    li a7, 64          # syscall write
    ecall
    
    # Exit
    li a0, 0           # exit code
    li a7, 93          # syscall exit
    ecall

.section .rodata
message:
    .asciz "Hello, RISC-V!
"
