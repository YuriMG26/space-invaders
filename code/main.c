#include <raylib.h>

#include "utils.h"

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
  // TODO: move this to GameBegin
  game->debug_mode = true;
  game->debug_rect = (Rectangle){10, 10, 350, 550};
  game->debug_font_size = 24;
  game->debug_font =
      LoadFontEx("assets/inconsolata.ttf", game->debug_font_size, 0, 0);

  game->logger = LoggerBegin("log.txt", true);

  while (!WindowShouldClose())
  {

    BeginDrawing();
    ClearBackground(BLACK);

    GameDraw(game);

    GameSimulate(game);

    GameDrawDebugInfo(game);

    EndDrawing();
  }

  LoggerEnd(game->logger);
  GameEnd(game);
  return 0;
}
