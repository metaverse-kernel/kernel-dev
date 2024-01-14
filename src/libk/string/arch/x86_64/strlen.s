    section .text

    global strlen
; usize strlen(char *)
strlen:
    push rcx
    
    cld
    mov al, 0
    xor rcx, rcx
    dec rcx
    repne scasb

    not rcx
    dec rcx

    mov rax, rcx
    pop rcx
    ret
