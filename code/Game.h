#pragma once

#include "Logger.h"
#include "utils.h"
#include <raylib.h>

typedef Rectangle rect;
typedef Texture tex;

enum AlienType
{
  NONE = 0,
  OCTOPUS,
  CRAB,
  SQUID,
  UFO,
  SHAPESHIFTER,
  BLINKING_UFO
};

typedef struct
{
  rect pos;
} Player;

typedef struct
{
  rect pos;
  rect outer_rectangle;
  u8 type;
  b32 alive;
} Alien;

typedef struct
{
  u16 id;
  b32 active;
  rect pos;
} Bullet;

#define ALIENS_PER_ROW 11
#define ALIENS_PER_COLUMN 5
#define ALIEN_NUM (ALIENS_PER_ROW * ALIENS_PER_COLUMN)

#define MAX_BULLETS 16

typedef struct
{
  Sound laser;
  Sound explosion;
} GameSound;

typedef struct
{
  Logger *logger;
  GameSound sound_assets;

  u16 width, height;       // Window dimensions
  char title[64];          // Window title
  Texture graphics;        // Spritesheet with all graphics.
  Player player;           // Player entity
  Alien aliens[ALIEN_NUM]; // Array of alien entities
  u16 aliens_alive_count;

  rect alien_rows[ALIENS_PER_COLUMN]; // For collision detection.

  Bullet bullets[MAX_BULLETS]; // Array of bullets

  u16 active_bullets; // Number of active bullets

  b32 debug_mode;
  rect debug_rect;
  Font debug_font;
  i32 debug_font_size;

  rect debug_full_rectangle;
} GameApp;

GameApp *GameBegin(int argc, char *argv[]);
void GameEnd(GameApp *game);

void GameSimulate(GameApp *game);
void GameDraw(GameApp *game);

void GameDrawDebugInfo(GameApp *game);

void Shoot(GameApp *game);
