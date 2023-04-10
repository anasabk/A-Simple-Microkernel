.section .text

.global _ZN11microkernel6Kernel4forkEv
_ZN11microkernel6Kernel4forkEv:
    push    ebp
    mov     esp, ebp
    push    ebx
    mov     $0x2, %eax
    mov     %edx, %ecx
    int     $0x80
    mov     %ebx, %eax
    iret