#include <kernel/arch/x86_64/interrupt_procs.h>
#include <kernel/arch/x86_64/memm.h>
#include <kernel/arch/x86_64/proc.h>
#include <kernel/kernel.h>
#include <kernel/tty.h>
#include <libk/string.h>

interrupt_req_gen(UNSUPPORTED)
{
    tty **tty0_option = tty_get(0);
    if (tty0_option == nullptr)
    {
        KERNEL_TODO();
    }
    tty *tty0 = *tty0_option;
    tty_text_print(tty0, "Panic: Unsupported interrupt.", RED, BLACK);
    KERNEL_TODO();
}

static void rip_not_cannonical(u64 rip, tty *tty0)
{
    char num[17];
    memset(num, 0, sizeof(num));
    pointer_to_string(rip, num);
    tty_text_print(tty0, "Panic", RED, BLACK);
    tty_text_print(tty0, ": Unexpected non-cannonical %rip value ", WHITE, BLACK);
    tty_text_print(tty0, num, ORANGE, BLACK);
    tty_text_print(tty0, ".\n", WHITE, BLACK);
    KERNEL_TODO();
}

interrupt_req_gen(DE)
{
    tty **tty0_option = tty_get(0);
    if (tty0_option == nullptr)
    {
        KERNEL_TODO();
    }
    tty *tty0 = *tty0_option;
    if (!is_cannonical(rip))
    {
        rip_not_cannonical(rip, tty0);
    }
    if (!is_user_address(rip))
    {
        char nums[34];
        memset(nums, 0, sizeof(nums));
        pointer_to_string(rip, nums);
        pointer_to_string(rsp, nums + 17);
        tty_text_print(tty0, "Panic", RED, BLACK);
        tty_text_print(tty0, ": Divided by zero occurs in kernel,\n\t", WHITE, BLACK);
        tty_text_print(tty0, "with %rip=", WHITE, BLACK);
        tty_text_print(tty0, "0x", ORANGE, BLACK);
        tty_text_print(tty0, nums, ORANGE, BLACK);
        tty_text_print(tty0, ", %rsp=", WHITE, BLACK);
        tty_text_print(tty0, "0x", ORANGE, BLACK);
        tty_text_print(tty0, nums + 17, ORANGE, BLACK);
        tty_text_print(tty0, ".\n", WHITE, BLACK);
        KERNEL_TODO();
    }
    else
    { // TODO 转储并结束进程
        KERNEL_TODO();
    }
}

interrupt_req_gen(BP)
{
    tty **tty0_option = tty_get(0);
    if (tty0_option == nullptr)
    {
        KERNEL_TODO();
    }
    tty *tty0 = *tty0_option;
    if (!is_cannonical(rip))
    {
        rip_not_cannonical(rip, tty0);
    }
    proc_texture_registers_t *texture = (void *)errcode;
    if (!is_user_address(rip))
    {
        char nums[34];
        memset(nums, 0, sizeof(nums));
        pointer_to_string(rip, nums);
        pointer_to_string(rsp, nums + 17);
        tty_text_print(tty0, "Debug", PURPLE, BLACK);
        tty_text_print(tty0, ": Kernel hit a breakpoint,\n\t", WHITE, BLACK);
        tty_text_print(tty0, "with %rip=", WHITE, BLACK);
        tty_text_print(tty0, "0x", ORANGE, BLACK);
        tty_text_print(tty0, nums, ORANGE, BLACK);
        tty_text_print(tty0, ", %rsp=", WHITE, BLACK);
        tty_text_print(tty0, "0x", ORANGE, BLACK);
        tty_text_print(tty0, nums + 17, ORANGE, BLACK);
        tty_text_print(tty0, ",\n\t", WHITE, BLACK);
        tty_text_print(tty0, "on texture: \n", WHITE, BLACK);
        pointer_to_string(texture->rax, nums);
        tty_text_print(tty0, "rax\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->rcx, nums);
        tty_text_print(tty0, "rcx\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->rbx, nums);
        tty_text_print(tty0, "rbx\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->rdx, nums);
        tty_text_print(tty0, "rdx\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->rsi, nums);
        tty_text_print(tty0, "rsi\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->rdi, nums);
        tty_text_print(tty0, "rdi\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->rbp, nums);
        tty_text_print(tty0, "rbp\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->rsp, nums);
        tty_text_print(tty0, "rsp\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->rip, nums);
        tty_text_print(tty0, "rip\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->rflags, nums);
        tty_text_print(tty0, "rflags\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->r8, nums);
        tty_text_print(tty0, "r8\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->r9, nums);
        tty_text_print(tty0, "r9\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->r10, nums);
        tty_text_print(tty0, "r10\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->r11, nums);
        tty_text_print(tty0, "r11\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->r12, nums);
        tty_text_print(tty0, "r12\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->r13, nums);
        tty_text_print(tty0, "r13\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->r14, nums);
        tty_text_print(tty0, "r14\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->r15, nums);
        tty_text_print(tty0, "r15\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->cs, nums);
        tty_text_print(tty0, "cs\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->ss, nums);
        tty_text_print(tty0, "ss\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        pointer_to_string(texture->ds, nums);
        tty_text_print(tty0, "ds\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\t", WHITE, BLACK);
        pointer_to_string(texture->es, nums);
        tty_text_print(tty0, "es\t", WHITE, BLACK);
        tty_text_print(tty0, nums, BLUE, BLACK);
        tty_text_print(tty0, "\n", WHITE, BLACK);
        KERNEL_TODO();
    }
    else
    { // TODO 将当前进程的状态设置为暂停并通知当前进程的调试程序
        KERNEL_TODO();
    }
}
