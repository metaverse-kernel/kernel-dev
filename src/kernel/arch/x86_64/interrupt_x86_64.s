    section .text

    global interrupt_open
interrupt_open:
    sti
    ret

    global interrupt_close
interrupt_close:
    cli
    ret

    global interrupt_rust_enter
    extern TSS
; usize interrupt_rust_enter()
; 返回值用于为寄存器rax占位
interrupt_rust_enter:
    ; *(rsp - 8) = *(TSS + 36)
    mov rax, [TSS + 36]
    mov qword [rsp - 8], rax
    ; *(TSS + 36) = rsp - 8
    mov rax, TSS
    lea rax, [rax + 36]
    mov [rax], rsp
    sub qword [rax], 8
    sti
    ret

    global interrupt_rust_leave
    extern TSS
; usize interrupt_rust_leave()
; 返回值用于为寄存器rax占位
interrupt_rust_leave:
    cli
    ; *(TSS + 36) = *(rsp - 8)
    mov rax, [rsp - 8]
    mov [TSS + 36], rax
    ret
