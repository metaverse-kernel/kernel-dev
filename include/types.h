#ifndef TYPES_H
#define TYPES_H 1

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef i64 isize;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef u64 usize;

typedef u8 bool;

#define false (0)
// 判断相等时不能使用true
#define true (!false)

#define nullptr ((void *)0)

#endif
