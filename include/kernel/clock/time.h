#ifndef TIME_H
#define TIME_H

#include <types.h>

// 使用UNIX时间戳
/**
 * @name system_time_get
 * 
 * ```c
 * usize system_time_get();
 * ```
 * 
 * 系统时间。
 * 
 * 系统时间使用精确到毫秒的unix时间，即值`0`代表1970-1-1 00:00:00的第一毫秒前。
 */
usize system_time_get();

// 如果硬件支持更高的计时精度，
// 此函数提供从系统unix时间开始到现在的纳秒为单位的时间
/**
 * @name system_time_ns_get
 * 
 * ```c
 * usize system_time_ns_get();
 * ```
 * 
 * 纳秒级系统时间。
 * 
 * 如果硬件支持更高的计时精度，此函数返回以纳秒为单位的时间。但不表明硬件必须支持纳秒级的计时。
 * 
 * 即使硬件支持更高的计时精度，内核也会根据情况自主选择是否使用更高的精度计时。
 */
usize system_time_ns_get();

/**
 * @name system_time_increase
 * 
 * ```c
 * void system_time_increase();
 * ```
 * 
 * 将毫秒级系统时间增加。
 */
void system_time_increase();

#endif
