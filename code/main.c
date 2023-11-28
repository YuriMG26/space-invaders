#include <raylib.h>

#include "Game.h"
#include "Logger.h"

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    LogError(NULL, "argument parsing is not implemented yet!");
    exit(EXIT_FAILURE);
  }

  GameApp *game = GameBegin(argc, argv);

  while (!WindowShouldClose())
  {

    BeginDrawing();
    ClearBackground(BLACK);

    GameDraw(game);
    GameSimulate(game);

    EndDrawing();
  }

  // LoggerEnd(game->logger);
  GameEnd(game);
  return 0;
}
