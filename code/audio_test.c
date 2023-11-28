#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

float sample_index = 0.0f;

bool tick_signal = false;
bool previous_frame_signal = false;

unsigned int signal_counter = 0;
unsigned int zero_counter = 0;

static void AudioProcess(void *buffer, unsigned int frames)
{
  short *d = (short *)buffer;
  previous_frame_signal = tick_signal;

  for (unsigned int i = 0; i < frames; i += 1)
  {
    if (d[i] != 0)
    {
      signal_counter += 1;
      if (signal_counter == 4)
      {
        tick_signal = true;
        signal_counter = 0;
      }
    }
    else
    {
      zero_counter++;
      if (zero_counter == 4)
      {
        tick_signal = false;
        zero_counter = 0;
      }
    }
  }
}

int main(void)
{
  InitWindow(800, 600, "audio test");
  InitAudioDevice();

  SetTargetFPS(60);

  Music m = LoadMusicStream("assets/background2.wav");

  AttachAudioStreamProcessor(m.stream, AudioProcess);

  double current_time = GetTime();

  PlayMusicStream(m);

  unsigned int num_of_aliens = 1;

  float pitch = 1.0;

  Color c;

  bool tick = false;
  bool tick_stopped = false;

  unsigned int tick_counter = 0;

  while (!WindowShouldClose())
  {
    float increment = num_of_aliens / 55.f;
    if (current_time + 0.3 < GetTime())
    {
      current_time = GetTime();
      num_of_aliens += 1.0f;
      if (num_of_aliens >= 55)
        num_of_aliens = 1.0f;
    }
    BeginDrawing();

    UpdateMusicStream(m);

    pitch = 1.0f + increment;
    SetMusicPitch(m, pitch);

    if (tick_signal)
    {
      c = (Color){255, 0, 0, 255};
      tick_counter++;
      if (tick_counter == 1)
      {
        printf("tick\n");
      }
      if (tick_stopped)
        tick = true;
    }
    else
    {
      tick_stopped = true;
      tick_counter = 0;
      c = RAYWHITE;
    }

    if (tick && !tick_stopped)
    {
      tick = false;
      printf("tick\n");
    }

    // printf("%s\n", (c.r == 255) ? "RED" : "WHITE");

    ClearBackground(c);
    EndDrawing();

    previous_frame_signal = tick_signal;
  }

  CloseWindow();
  return 0;
}
