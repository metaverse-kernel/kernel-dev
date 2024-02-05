    section .entry  align=8
    extern kmain
    global init64
init64:
    endbr64
    cli
    mov rax, 0x1000000
    mov rbp, rax
    mov rsp, rax
    mov rdi, rbx
    jmp kmain

    section .multiboot2 align=8
    MULTIBOOT2_HEADER_MAGIC    equ 0xe85250d6
    MULTIBOOT2_HEADER_LENGTH   equ multiboot2_header_end - multiboot2_header
multiboot2_header:
    dd MULTIBOOT2_HEADER_MAGIC
    dd 0
    dd MULTIBOOT2_HEADER_LENGTH
    dd - (MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_HEADER_LENGTH)
    bootinfo_request_tag:
        dd 1
        dd bootinfo_request_tag_end - bootinfo_request_tag
        dd 6    ; memory map
        dd 1    ; boot command line
        dd 8    ; framebuffer info
        dd 12   ; efi64 system table pointer
    bootinfo_request_tag_end:
    framebuffer_tag:
        dd 5
        dd 20
        dd 1920 ; width
        dd 1080 ; height
        dd 32   ; depth
    framebuffer_tag_end:
    dd  0
    end_tag:
        dd 0
        dd 8
multiboot2_header_end:

    section .kend
    global kend
kend:
    db 0
