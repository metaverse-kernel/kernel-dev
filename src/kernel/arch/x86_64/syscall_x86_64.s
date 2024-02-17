    section .data
    global system_calls_table
system_calls_table:
    resq 256

kernel_stack_cache:
    dq 0, 0     ; 分别为 rbp, rsp

    section .text
    global systemcall_procedure
    global save_kernel_stack
systemcall_procedure:
    endbr64
    call load_kernel_stack
    push rbp
    mov rbp, rsp

    shl rax, 3      ; rax *= 8
    ; 将对应调用号的系统调用加载至rax
    lea rdi, [system_calls_table]
    lea rax, [rax + rdi]
    ; 判断是否为空调用
    cmp rax, 0
    je systemcall_procedure_none_call
    mov rax, [rax]
    ; 调用对应的系统调用
    call rax

    leave
    call save_kernel_stack
    sysret

systemcall_procedure_none_call:
    ; TODO 调用了不存在的系统调用，属于无法恢复的错误，应保存错误状态并结束调用进程
    ; 暂时直接sysret
    leave
    call save_kernel_stack
    sysret

; void set_kernel_stack_cache(usize stack)
set_kernel_stack_cache:
    endbr64
    push rax

    lea rax, [kernel_stack_cache]
    mov [rax], rdi
    lea rax, [rax + 8]
    mov [rax], rdi

    pop rax
    ret

; void return_from_systemcall()
return_from_systemcall:
    sysret

save_kernel_stack:
    endbr64
    lea rbx, [kernel_stack_cache]
    ; 交换[rbx]与rbp
    mov rdi, [rbx]
    xor rbp, rdi
    xor rdi, rbp
    xor rbp, rdi
    mov [rbx], rdi
    lea rbx, [rbx + 8]
    ; 交换[rbx]与rsp
    mov rdi, [rbx]
    xor rsp, rdi
    xor rdi, rsp
    xor rsp, rdi
    mov [rbx], rdi
    ret

load_kernel_stack:
    endbr64
    lea rbx, [kernel_stack_cache]
    ; 交换[rbx]与rbp
    mov rdi, [rbx]
    xor rbp, rdi
    xor rdi, rbp
    xor rbp, rdi
    mov [rbx], rdi
    lea rbx, [rbx + 8]
    ; 交换[rbx]与rsp
    mov rdi, [rbx]
    xor rsp, rdi
    xor rdi, rsp
    xor rsp, rdi
    mov [rbx], rdi
    ret
