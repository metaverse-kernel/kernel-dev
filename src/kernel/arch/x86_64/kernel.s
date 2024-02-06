    section .text
    global prepare_stack
prepare_stack:
    push rdx
    push rbx

    mov rax, rsp
    add rax, 16
    xor rdx, rdx
    mov rbx, 16
    div rbx
    mov rax, rdx

    pop rbx
    pop rdx
    sub rsp, rax
    add rax, rsp
    sub rsp, 16
    mov rax, [rax]
    mov [rsp], rax
    ret

    global interrupt_open
interrupt_open:
    sti
    ret

    global interrupt_close
interrupt_close:
    cli
    ret
