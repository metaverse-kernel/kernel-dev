    section .text
    global reload_pml4
reload_pml4:
    push rax

    mov rax, cr3
    mov cr3, rax

    pop rax

    ret
