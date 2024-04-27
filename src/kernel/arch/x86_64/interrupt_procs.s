%include "arch/x86_64/interrupt.in"

; 栈中的寄存器上下文
; +------------
; | ss
; +------------<-- rsp+168(+160)
; | rsp
; +------------<-- rsp+160(+152)
; | rflags
; +------------<-- rsp+152(+144)
; | cs
; +------------<-- rsp+144(+136)
; | rip
; +------------<-- rsp+136(+128)
; | (err code)
; +------------<-- rsp+128
; | rbp
; +------------<-- rsp+120
; | rax
; +------------<-- rsp+112
; | rcx
; +------------<-- rsp+104
; | rbx
; +------------<-- rsp+96
; | rdx
; +------------<-- rsp+88
; | rsi
; +------------<-- rsp+80
; | rdi
; +------------<-- rsp+72
; | r8
; +------------<-- rsp+64
; | r9
; +------------<-- rsp+56
; | r10
; +------------<-- rsp+48
; | r11
; +------------<-- rsp+40
; | r12
; +------------<-- rsp+32
; | r13
; +------------<-- rsp+24
; | r14
; +------------<-- rsp+16
; | r15
; +------------<-- rsp+8
; | reserved
; +------------<-- rsp+4
; | ds
; +------------<-- rsp+2
; | es
; +------------<-- rsp+0

    section .text
    
    global interrupt_entry_UNSUPPORTED
    extern interrupt_req_UNSUPPORTED
interrupt_entry_UNSUPPORTED:
    interrupt_entry_enter

    mov rdi, [rsp + 128]
    mov rsi, [rsp + 152]
    call interrupt_req_UNSUPPORTED

    interrupt_entry_leave
    iret

    global interrupt_entry_DE
    extern interrupt_req_DE
interrupt_entry_DE:
    interrupt_entry_enter

    mov rdi, [rsp + 128]
    mov rsi, [rsp + 152]
    call interrupt_req_DE

    interrupt_entry_leave
    iret

    global interrupt_entry_NMI
interrupt_entry_NMI:
    ; TODO 暂时不需要为这个中断实现任何功能
    iret

    global interrupt_entry_BP
    extern interrupt_req_BP
interrupt_entry_BP:
    interrupt_entry_enter

    mov rdi, [rsp + 128]
    mov rsi, [rsp + 152]
    mov dword [rsp + 4], 0
    mov rdx, rsp
    call interrupt_req_BP

    interrupt_entry_leave
    iret

    global interrupt_entry_OF
    extern interrupt_req_OF
interrupt_entry_OF:
    interrupt_entry_enter

    mov rdi, [rsp + 128]
    mov rsi, [rsp + 152]
    call interrupt_req_OF

    interrupt_entry_leave
    iret
