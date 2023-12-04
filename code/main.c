#include "Game.h"
#include <raylib.h>

#if RELEASE_BUILD == 1
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
int main(int argc, char *argv[])
{
  GameApp *game = GameBegin(argc, argv);
  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);
    GameDraw(game);
    GameSimulate(game);
    EndDrawing();
  }
  GameEnd(game);
}
