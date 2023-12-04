#pragma once

#include <stdio.h>

#if LOGGER
#define assert(expr)                                                           \
  if ((expr))                                                                  \
  {                                                                            \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    LogError(NULL, "assertion failure: " LOG_RED "(%s)" LOG_RESET " at %s:%d", \
             #expr, __FILE__, __LINE__);                                       \
  }
#else
#define assert(expr)                                                           \
  if ((expr))                                                                  \
  {                                                                            \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    printf("assertion failure: (%s) at %s:%d", #expr, __FILE__, __LINE__);     \
  }
#endif
