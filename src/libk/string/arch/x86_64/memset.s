    section .text

    global memset
; void memset(void *__dest, u8 __src, usize len)
memset:
    endbr64

    sub rsp, 0x10
    mov [rsp], rax
    mov [rsp + 8], rcx

    cld
    mov rax, rsi
    mov rcx, rdx
    rep stosb

    mov rax, [rsp]
    mov rcx, [rsp + 8]
    add rsp, 0x10

    ret
