#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define _1d_index_to_2d(width, row, col) (width * row + col)

#define ALWAYS_INLINE __attribute__((always_inline))

#include "MyTypes.h"

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

#define DEBUG_ASSERT(cond)                                                     \
  if (!(cond))                                                                 \
  {                                                                            \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    __debugbreak();                                                            \
  }
