    section .text

    global memcpy
; void memcpy(void *__dest, restrict void *__src, usize len)
memcpy:
    endbr64
    sub rsp, 8
    mov [rsp], rcx

    cld
    mov rcx, rdx
    rep movsb

    mov rcx, [rsp]
    add rsp, 8

    ret
