#include "MyString.h"
#include "Arena.h"
#include "Assert.h"
#include <stdlib.h>
#include <string.h>

void mStringLenNullTerminated(const char *s, u64 *len)
{
  for (*len = 0; s[*len]; (*len)++)
    ;
}

// Default allocator (malloc)
mString mStringBegin(const char *s)
{
  mString result = {0};
  mStringLenNullTerminated(s, &result.len);
  result.s = (char *)malloc(sizeof(result.len) + 1);
  strcpy_s(result.s, result.len + 1, s);
  return result;
}

mString mStringBeginWithArena(Arena *arena, const char *s)
{
  assert(arena != NULL);
  mString result = {0};
  mStringLenNullTerminated(s, &result.len);
  result.s = (char *)ArenaAlloc(sizeof(result.len) + 1);
  strcpy_s(result.s, result.len + 1, s);
  return result;
}
