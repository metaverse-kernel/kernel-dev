#include <kernel/clock/time.h>
#include <types.h>

usize system_time;
usize system_time_ns;

usize system_time_get()
{
    return system_time;
}

usize system_time_ns_get()
{
    return system_time_ns;
}

void system_time_increase()
{
    system_time++;
}
