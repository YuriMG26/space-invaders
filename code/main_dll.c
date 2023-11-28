#include <raylib.h>

#include "Game.h"
#include "Logger.h"

// #include "win.h"
#define _AMD64_ 1
#include <windef.h>

#include <fileapi.h>
#include <libloaderapi.h>
#include <winbase.h>

#include "ray_functions.h"

typedef struct
{
  FILETIME time;
  HMODULE dll;
  char file_name[48];
} DLLWithTime;

// typedef GameApp *(*GameCodeBegin)(RayFunctions ray);
// typedef void (*GameCodeEnd)(GameApp *game);
// typedef void (*GameCodeDraw)(GameApp *game);
// typedef void (*GameCodeSimulate)(GameApp *game);

typedef struct
{
  DLLWithTime temp_game_code;

  game_begin_t *game_begin;
  game_end_t *game_end;
  game_draw_t *game_draw;
  game_simulate_t *game_simulate;
} GameCode;

internal FILETIME GetLastWriteTime(const char *path)
{
  FILETIME result = {};

  WIN32_FIND_DATA find_data;
  HANDLE find_handle = FindFirstFileA(path, &find_data);
  if (find_handle != INVALID_HANDLE_VALUE)
  {
    result = find_data.ftLastWriteTime;
    FindClose(find_handle);
  }
  else
  {
  }
  return result;
}

internal DLLWithTime LoadDLLWithTime(const char *path)
{
  DLLWithTime result = {0};
  strcpy_s(result.file_name, 48, path);

  result.time = GetLastWriteTime(result.file_name);
  result.dll = LoadLibrary(path);

  return result;
}

internal GameCode LoadGameCode(void)
{
  GameCode result = {0};

  CopyFile("game.dll", "temp_game.dll", FALSE);
  result.temp_game_code = LoadDLLWithTime("temp_game.dll");

  result.game_begin =
      (game_begin_t *)GetProcAddress(result.temp_game_code.dll, "GameBegin");
  result.game_end =
      (game_end_t *)GetProcAddress(result.temp_game_code.dll, "GameEnd");
  result.game_draw =
      (game_draw_t *)GetProcAddress(result.temp_game_code.dll, "GameDraw");
  result.game_simulate = (game_simulate_t *)GetProcAddress(
      result.temp_game_code.dll, "GameSimulate");

  return result;
}

internal RayFunctions LoadRaylibCode()
{
  RayFunctions result = {0};

  result.InitWindow = InitWindow;
  result.InitWindow = InitWindow;
  result.CloseWindow = CloseWindow;
  result.InitAudioDevice = InitAudioDevice;
  result.GetFrameTime = GetFrameTime;
  result.GetFPS = GetFPS;
  result.GetTime = GetTime;

  result.GetRandomValue = GetRandomValue;

  result.SetConfigFlags = SetConfigFlags;
  result.LoadFontEx = LoadFontEx;

  result.SetTargetFPS = SetTargetFPS;
  result.GetMonitorRefreshRate = GetMonitorRefreshRate;
  result.GetCurrentMonitor = GetCurrentMonitor;

  result.GetScreenWidth = GetScreenWidth;
  result.GetScreenHeight = GetScreenHeight;

  result.LoadTexture = LoadTexture;
  result.LoadSound = LoadSound;
  result.UnloadTexture = UnloadTexture;
  result.SetSoundVolume = SetSoundVolume;

  result.LoadMusic = LoadMusicStream;
  result.PlayMusic = PlayMusicStream;
  result.IsMusicReady = IsMusicReady;
  result.IsMusicStreamPlaying = IsMusicStreamPlaying;
  result.SetMusicPitch = SetMusicPitch;
  result.UpdateMusicStream = UpdateMusicStream;
  result.ResumeMusicStream = ResumeMusicStream;
  result.LoadWave = LoadWave;

  result.PlaySound = PlaySound;
  result.LoadSoundFromWave = LoadSoundFromWave;
  result.WaveFormat = WaveFormat;

  result.GetApplicationDirectory = GetApplicationDirectory;

  result.DrawTexturePro = DrawTexturePro;
  result.DrawRectangleRec = DrawRectangleRec;
  result.DrawRectangleRounded = DrawRectangleRounded;
  result.DrawRectangleLines = DrawRectangleLines;
  result.DrawRectangleLinesEx = DrawRectangleLinesEx;

  result.DrawText = DrawText;
  result.DrawTextEx = DrawTextEx;
  result.MeasureText = MeasureText;

  result.IsKeyDown = IsKeyDown;
  result.IsKeyPressed = IsKeyPressed;
  result.GetMousePosition = GetMousePosition;
  result.IsMouseButtonDown = IsMouseButtonDown;
  result.GetMouseDelta = GetMouseDelta;

  result.CheckCollisionRecs = CheckCollisionRecs;
  result.CheckCollisionPointRec = CheckCollisionPointRec;

  return result;
}

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    LogError(NULL, "argument parsing is not implemented yet!");
    exit(EXIT_FAILURE);
  }
  GameCode game_code = LoadGameCode();
  RayFunctions ray = LoadRaylibCode();

  GameApp *game = game_code.game_begin(ray, argc, argv);

  while (!WindowShouldClose())
  {
    FILETIME ft = GetLastWriteTime("game.dll");
    if (CompareFileTime(&game_code.temp_game_code.time, &ft) != 0)
    {
      LogInfo(game->logger, "reloading game code");
      FreeLibrary(game_code.temp_game_code.dll);
      game_code.temp_game_code.dll = NULL;
      game_code = LoadGameCode();
      game_code.temp_game_code.time = ft;

      game->game_code_was_reloaded = true;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    game_code.game_draw(game);
    game_code.game_simulate(game);

    EndDrawing();
  }

  LoggerEnd(game->logger);
  game_code.game_end(game);
  return 0;
}
