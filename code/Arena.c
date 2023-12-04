#include "Arena.h"

void ArenaInit(Arena *a, void *backing_buffer, u64 backing_buffer_length)
{
}
void *ArenaResizeAlign(Arena *a, void *old_memory, u64 old_size, u64 new_size,
                       u64 align)
{
}

void ArenaFreeAll(Arena *a)
{
}

void *ArenaAlloc(u64 size)
{
}

void *ArenaAllocAlign(Arena *a, u64 size, u64 align)
{
}

uptr AlignForward(uptr ptr, u64 align)
{
}
