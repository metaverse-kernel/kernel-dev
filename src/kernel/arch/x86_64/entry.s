    section .entry  align=8
    extern kmain
    global init64
init64:
    endbr64
    cli
    jmp $
    mov rax, 0x1000000
    mov rbp, rax
    mov rsp, rax
    mov rdi, rbx
    jmp qword kmain

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
        dd 4    ; basic mamory info
        dd 1    ; boot command line
        dd 3    ; muldules
        dd 6    ; memory map
        dd 8    ; framebuffer info
        dd 12   ; efi64 system table pointer
    bootinfo_request_tag_end:
    end_tag:
        dd 0
        dd 8
multiboot2_header_end:

    section .bss
kstack:
    ; TODO 奇怪的问题，在这写0x1000000 - 0x400000，.bss段会超出0x47f，
    ; 写0x1000000 - 0x400000 - 0x47f，.bss段会超出0x1f
    ; 明明在链接脚本里.bss段的起始地址是0x400000
    resb 0x1000000 - 0x400000 - 0x47f - 0x1f

    section .kend
    global kend
kend:
    db 0
