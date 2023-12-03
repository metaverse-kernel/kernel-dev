#ifndef KLOG_H
#define KLOG_H 1

#include <types.h>

#define KLOG_BUFFER_SIZE 2 * 1024 * 1024

typedef struct __klogger
{
    usize tty;
    char buffer[KLOG_BUFFER_SIZE];
    usize index;
} klogger;

#endif
