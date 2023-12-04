#pragma once
#include "utils.h"
#include <raylib.h>

typedef struct
{
  Rectangle frame;
  f32 duration; // in seconds
} Frame;

typedef struct
{
  u32 capacity;
  u32 size;
  Frame *frames;
} FrameArray;

// TODO: implement this
typedef struct
{
  f64 starting_time;
  f64 ending_time;
  b32 looping;
  Rectangle *current_frame; // TODO/NOTE: is pointer here safe???
  FrameArray frame_array;
} Animator;
