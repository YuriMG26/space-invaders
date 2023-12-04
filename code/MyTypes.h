#pragma once
#include <stdbool.h>
#include <stdint.h>

#define global static
#define external
#define internal static
#define local_persist static

typedef uintptr_t uptr;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef u32 b32;
