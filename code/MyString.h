#pragma once

#include "MyTypes.h"

typedef struct
{
  char *s;
  u64 len;
} mString;

mString StringBegin(const char *s);
