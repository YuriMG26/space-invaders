#include "Logger.h"
#include <stdarg.h>
#include <stdio.h>

Logger *LoggerBegin(const char *filename, b32 write_to_file)
{
  Logger *result = AllocAndZero(sizeof(Logger));
  if (write_to_file != false)
  {
    if (filename != NULL)
    {
      fopen_s(&result->handle, filename, "w");
      if (result->handle != NULL)
      {
        result->write_to_file = true;
        strcpy_s(result->filename, 64, filename);
        LogInfo(result, "Logger initialized successfully on file \"%s\"",
                result->filename);
      }
      else
      {
        LogFatalError(NULL,
                      "FATAL ERROR: file \"%s\" provided to logger could "
                      "not be opened.\n",
                      filename);
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      LogFatalError(NULL, "FATAL ERROR: invalid filename provided to logger.");
      exit(EXIT_FAILURE);
    }
  }

  return result;
}

void LoggerEnd(Logger *logger)
{
  if (logger->write_to_file)
  {
    fclose(logger->handle);
  }
  free(logger);
}

void LogInfo(Logger *logger, const char *fmt, ...)
{
  char buffer[256];
  snprintf(buffer, 128, LOG_GREEN "[INFO]:" LOG_RESET " %s\n", fmt);

  va_list args;
  va_start(args, fmt);
  vprintf_s(buffer, args);
  if (logger != NULL && logger->write_to_file)
  {
    vfprintf_s(logger->handle, buffer, args);
  }
  va_end(args);
}

void LogError(Logger *logger, const char *fmt, ...)
{
  char buffer[256];
  snprintf(buffer, 128, LOG_RED "[ERROR]:" LOG_RESET " %s\n", fmt);

  va_list args;
  va_start(args, fmt);
  vprintf_s(buffer, args);
  if (logger != NULL && logger->write_to_file)
  {
    vfprintf_s(logger->handle, buffer, args);
  }
  va_end(args);
}

void LogFatalError(Logger *logger, const char *fmt, ...)
{
  char buffer[256];
  snprintf(buffer, 128,
           LOG_RED "[FATAL ERROR]:" LOG_RESET " %s." LOG_RED
                   " CRASHING...\n" LOG_RESET,
           fmt);

  va_list args;
  va_start(args, fmt);
  vprintf_s(buffer, args);
  if (logger != NULL && logger->write_to_file)
  {
    vfprintf_s(logger->handle, buffer, args);
  }
  va_end(args);
  exit(EXIT_FAILURE);
}

void LogWarning(Logger *logger, const char *fmt, ...)
{
  char buffer[256];
  snprintf(buffer, 128, LOG_YELLOW "[WARNING]:" LOG_RESET " %s\n", fmt);

  va_list args;
  va_start(args, fmt);
  vprintf_s(buffer, args);
  if (logger != NULL && logger->write_to_file)
  {
    vfprintf_s(logger->handle, buffer, args);
  }
  va_end(args);
}
