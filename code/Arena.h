#pragma once

#include "MyTypes.h"

/*
 * Simple implementation for an arena allocator, used for temporary allocations
 * based on the article by GingerBill:
 * https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

typedef struct
{
  u8 *buffer;
  u64 length;
  u64 previous_offset;
  u64 current_offset;
} Arena;

inline bool IsPowerOfTwo(uptr x)
{
  return (x & (x - 1)) == 0;
}

void ArenaInit(Arena *a, void *backing_buffer, u64 backing_buffer_length);
void *ArenaResizeAlign(Arena *a, void *old_memory, u64 old_size, u64 new_size,
                       u64 align);
void ArenaFreeAll(Arena *a);
void *ArenaAlloc(u64 size);
void *ArenaAllocAlign(Arena *a, u64 size, u64 align);
uptr AlignForward(uptr ptr, u64 align);
