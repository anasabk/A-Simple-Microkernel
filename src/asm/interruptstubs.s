.set IRQ_BASE, 0x20

.section .text

.extern _ZN11microkernel16InterruptManager17receive_interruptEhj


.macro HandleException num
.global _ZN11microkernel16InterruptManager21handle_exception_\num\()Ev
_ZN11microkernel16InterruptManager21handle_exception_\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm


.macro HandleInterruptRequest num
.global _ZN11microkernel16InterruptManager29handle_interrupt_request_\num\()Ev
_ZN11microkernel16InterruptManager29handle_interrupt_request_\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0
    jmp int_bottom
.endm

.global _ZN11microkernel16InterruptManager29handle_interrupt_request_0x80Ev
_ZN11microkernel16InterruptManager29handle_interrupt_request_0x80Ev:
    movb $0x80, (interruptnumber)
    pushl $0
    jmp int_bottom

#.global _ZN11microkernel6Kernel4forkEv
#_ZN11microkernel6Kernel4forkEv:
#    push    %ebp
#    mov     %esp, %ebp
#    push    %ebx
#    mov     $0x2, %eax
#    mov     %edx, %ecx
#    int     $0x80
#    mov     %ebx, %eax
#    leave
#    iret


HandleException 0x00
HandleException 0x01
HandleException 0x02
HandleException 0x03
HandleException 0x04
HandleException 0x05
HandleException 0x06
HandleException 0x07
HandleException 0x08
HandleException 0x09
HandleException 0x0A
HandleException 0x0B
HandleException 0x0C
HandleException 0x0D
HandleException 0x0E
HandleException 0x0F
HandleException 0x10
HandleException 0x11
HandleException 0x12
HandleException 0x13

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x02
HandleInterruptRequest 0x03
HandleInterruptRequest 0x04
HandleInterruptRequest 0x05
HandleInterruptRequest 0x06
HandleInterruptRequest 0x07
HandleInterruptRequest 0x08
HandleInterruptRequest 0x09
HandleInterruptRequest 0x0A
HandleInterruptRequest 0x0B
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x0D
HandleInterruptRequest 0x0E
HandleInterruptRequest 0x0F
HandleInterruptRequest 0x31

#HandleInterruptRequest 0x80


int_bottom:

    # Save register
    pushl %ds
    
    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # Call C++ Handler
    pushl %esp
    push (interruptnumber)
    call _ZN11microkernel16InterruptManager17receive_interruptEhj
    mov %eax, %esp # Switch the stack

    # Restore register
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp

    pop %ds

    add $4, %esp


.global _ZN11microkernel16InterruptManager16ignore_interruptEv
_ZN11microkernel16InterruptManager16ignore_interruptEv:

    iret


.data
    interruptnumber: .byte 0