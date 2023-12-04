#include "Game.h"
#include <raylib.h>

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
