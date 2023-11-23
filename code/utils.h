#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define _1d_index_to_2d(width, row, col) (width * row + col)

#define ALWAYS_INLINE __attribute__((always_inline))

#define global static
#define external
#define internal static
#define local_persist static

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

typedef struct
{
  union
  {
    struct
    {
      f32 x, y;
    };
    float data[2];
  };
} v2;

inline f32 Centralize(f32 window_dimension, f32 width)
{
  return ((float)window_dimension / 2) - (width / 2);
}

inline void *AllocAndZero(size_t bytes)
{
  void *result = malloc(bytes);
  memset(result, 0, bytes);
  return result;
}
