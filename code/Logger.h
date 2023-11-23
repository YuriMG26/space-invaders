#pragma once

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define LOG_GREEN "\033[0m"
#define LOG_RED "\033[0;31m"
#define LOG_YELLOW "\033[1;33m"
#define LOG_RESET "\033[0m"

typedef struct
{
  b32 write_to_file;
  char filename[64];
  FILE *handle;
} Logger;

Logger *LoggerBegin(const char *filename, b32 write_to_file);
void LoggerEnd(Logger *logger);

void LogInfo(Logger *logger, const char *fmt, ...);
void LogError(Logger *logger, const char *fmt, ...);
void LogFatalError(Logger *logger, const char *fmt, ...);
void LogWarning(Logger *logger, const char *fmt, ...);

#define assert(expr)                                                           \
  if ((expr))                                                                  \
  {                                                                            \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    LogError(NULL, "assertion failure: " LOG_RED "(%s)" LOG_RESET " at %s:%d", \
             #expr, __FILE__, __LINE__);                                       \
  }
