#pragma once

#include <raylib.h>

typedef struct
{
  void (*InitWindow)(int width, int height, const char *title);
  void (*CloseWindow)(void);
  void (*InitAudioDevice)();
  float (*GetFrameTime)();
  int (*GetFPS)();
  double (*GetTime)();

  int (*GetRandomValue)(int min, int max);

  void (*SetConfigFlags)(unsigned int flags);
  Font (*LoadFontEx)(const char *fileName, int fontSize, int *codepoints,
                     int codepointCount);

  void (*SetTargetFPS)(int fps);
  int (*GetMonitorRefreshRate)(int monitor);
  int (*GetCurrentMonitor)(void);

  int (*GetScreenWidth)();
  int (*GetScreenHeight)();

  Texture (*LoadTexture)(const char *path);
  Sound (*LoadSound)(const char *path);
  void (*UnloadTexture)(Texture2D texture);
  void (*SetSoundVolume)(Sound sound, float volume);
  void (*PlaySound)(Sound sound);

  Sound (*LoadSoundFromWave)(Wave wave);
  void (*WaveFormat)(Wave *wave, int sampleRate, int sampleSize, int channels);

  Music (*LoadMusic)(const char *path);
  void (*PlayMusic)(Music music);
  bool (*IsMusicReady)(Music music);
  void (*UpdateMusicStream)(Music music);
  bool (*IsMusicStreamPlaying)(Music music);
  void (*SetMusicPitch)(Music music, float pitch);
  void (*ResumeMusicStream)(Music music);

  Wave (*LoadWave)(const char *fileName);

  const char *(*GetApplicationDirectory)();

  void (*DrawTexturePro)(Texture2D texture, Rectangle source, Rectangle dest,
                         Vector2 origin, float rotation, Color tint);
  void (*DrawRectangleRec)(Rectangle rec, Color tint);
  void (*DrawRectangleRounded)(Rectangle rec, float roundness, int sections,
                               Color tint);
  void (*DrawRectangleLines)(int x, int y, int width, int height, Color tint);
  void (*DrawRectangleLinesEx)(Rectangle rec, float thickness, Color tint);

  void (*DrawText)(const char *text, int posX, int posY, int fontSize,
                   Color color);
  void (*DrawTextEx)(Font font, const char *text, Vector2 position,
                     float fontSize, float spacing, Color tint);
  int (*MeasureText)(const char *text, int fontSize);

  bool (*IsKeyDown)(int key);
  bool (*IsKeyPressed)(int key);
  Vector2 (*GetMousePosition)(void);
  bool (*IsMouseButtonDown)(int button);
  Vector2 (*GetMouseDelta)(void);

  bool (*CheckCollisionRecs)(Rectangle rec1, Rectangle rec2);
  bool (*CheckCollisionPointRec)(Vector2 point, Rectangle rec);

} RayFunctions;
