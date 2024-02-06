#ifndef SYSCALL_H
#define SYSCALL_H 1

#ifdef __x86_64__
#include <kernel/arch/x86_64/syscall.h>
#endif

void syscall_init();

#endif
