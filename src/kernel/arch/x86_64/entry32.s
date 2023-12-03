    section .entry
    extern init64
    ; 寄存器ebx是multiboot2 information，不可以使用
init32:
    cli

    ; 设置PML4的第0项
    mov eax, 0x102000
    add eax, 0x003
    mov edi, 0x101000
    mov dword [edi], eax
    add edi, 4
    mov dword [edi], 0

    ; 设置PDPT的第0项
    mov eax, 0x103000
    add eax, 0x003
    mov edi, 0x102000
    mov dword [edi], eax
    add edi, 4
    mov dword [edi], 0

    ; 设置PD0中的PDE
    mov ecx, 64
    mov eax, 0
    mov edi, 0x103000
    init32_loop0:
        mov edx, eax
        add edx, 0x183
        mov dword [edi], edx
        add eax, 0x400000   ; 2MB
        add edi, 4
        mov dword [edi], 0
        add edi, 4
    loop init32_loop0

    ; 加载GDTR和段寄存器
    mov ax, 0x10
    mov ds, ax
    ; mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    db 0x66
    lgdt [0x104028]

    ; 打开PAE
    mov eax, cr4
    bts eax, 5
    mov cr4, eax

    ; 加载cr3
    mov eax, 0x101000
    mov cr3, eax

    ; 切换ia32e模式
    mov ecx, 0xc0000080 ; ia32_efer在msr中的地址
    rdmsr
    bts eax, 8
    wrmsr

    ; 打开分页机制
    mov eax, cr0
    bts eax, 31
    mov cr0, eax

    ; 远跳转
    jmp 0x8:init64

    section .cpumeta align=4096

    ; 分页
    ; 链接器会把这些数据替换掉所以要在代码中重新设置
PML4:
    dq  0x003 + PDPT0
    resq 511

PDPT0:
    dq  0x003 + PD0
    resq 511

PD0:
    resq 512

    ; 分段
gdt:
    dq  0
    dq  0x0020980000000000   ; 内核态代码段
    dq  0x0000920000000000   ; 内核态数据段
    dq  0x0020f80000000000   ; 用户态代码段
    dq  0x0000f20000000000   ; 用户态数据段
gdt_end:

gdt_ptr:
    dw  gdt_end - gdt - 1
    dq  gdt
