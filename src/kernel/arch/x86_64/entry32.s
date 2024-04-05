    section .entry
    extern init64
    ; 寄存器ebx是multiboot2 information，不可以使用
init32:
    cli

    ; 设置PML4的第0项
    mov eax, 0x102000   ; PDPT0
    add eax, 0x003
    mov edi, 0x101000   ; PML4
    mov dword [edi], eax
    add edi, 4
    mov dword [edi], 0

    ; 设置PDPT的第0项
    mov eax, 0x103000   ; PD0
    add eax, 0x003
    mov edi, 0x102000   ; PDPT0
    mov dword [edi], eax
    add edi, 4
    mov dword [edi], 0

    ; 设置PD0中的PDE
    mov ecx, 64
    mov eax, 0
    mov edi, 0x103000   ; PD0
    init32_loop0:
        mov edx, eax
        add edx, 0x183
        mov dword [edi], edx
        add eax, 0x200000   ; 2MB
        add edi, 4
        mov dword [edi], 0
        add edi, 4
    loop init32_loop0

    ; 加载GDTR、段寄存器和TR寄存器
    db 0x66
    lgdt [0x104000]     ; gdt_ptr
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ax, 0x30
    ltr ax

    ; 打开PAE
    mov eax, cr4
    bts eax, 5
    mov cr4, eax

    ; 加载cr3
    mov eax, 0x101000   ; PML4
    mov cr3, eax

    ; 切换ia32e模式
    mov ecx, 0xc0000080 ; ia32_efer在msr中的地址
    rdmsr
    bts eax, 8
    wrmsr

    ; 打开保护模式和分页机制
    mov eax, cr0
    bts eax, 0
    bts eax, 31
    mov cr0, eax

    ; 远跳转
    jmp 0x8:init64

    section .cpumeta align=4096
    global PML4
    ; 分页
PML4:
    dq  0x003 + PDPT0
    resq 511

PDPT0:
    dq  0x003 + PD0
    resq 511

PD0:
    resq 512

    section .cpumeta.tblptrs

gdt_ptr:    ; 0x104000
    dw  gdt_end - gdt - 1
    dq  gdt

    dd 0
    dw 0

idt_ptr:    ; 0x104010
    dw 0x7ff
    dq idt

    section .cpumeta.gdt align=4096
gdt:
    dq  0
    dq  0x0020980000000000  ; 内核态代码段
    dq  0x0000920000000000  ; 内核态数据段
    dq  0x0020f80000000000  ; 用户态代码段
    dq  0x0000f20000000000  ; 用户态数据段
    dq  0
    dq  0x0000891070000068  ; TSS段（低64位）
    dq  0                   ; TSS段（高64位）
gdt_end:

    section .cpumeta.idt align=4096
    global idt
idt:
    resq 512    ; 16 bytes per descriptor (512 q-bytes)

    section .cpumeta.tss align=4096
    global TSS
TSS:
    dd 0
    dq 0x1000000    ; （rsp0）内核栈
    dq 0            ; （rsp1）
    dq 0            ; （rsp2）
    dq 0
    dq 0x400000     ; （ist1）中断栈
    resb 104 - ($ - TSS)
