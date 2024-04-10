#include <kernel/syscall.h>

#include <libk/string.h>

void syscall_init()
{
    memset(&system_calls_table, 0, sizeof(system_calls_table));
}
