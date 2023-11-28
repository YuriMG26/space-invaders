#pragma once

#include "Logger.h"
#include "utils.h"
#include <raylib.h>
#include <raymath.h>

#include "ray_functions.h"
typedef Rectangle rect;
typedef Texture tex;

enum MovingTo
{
  LEFT,
  RIGHT,
  DOWN
};

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
  rect hitbox;
} Player;

typedef struct
{
  u16 id;
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

#define MAX_BULLETS 2

typedef struct
{
  union
  {
    struct
    {
      rect top_left;
      rect top_center;
      rect top_right;
      rect center_left;
      rect center_center;
      rect center_right;
      rect bottom_left;
      rect bottom_center;
      rect bottom_right;
    };
    rect rectangles[9];
  };
  rect pos;
} Barrier;

typedef struct
{
  Sound laser;
  Sound explosion;
  // Music background;
  Music background_wave;
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

  Barrier barriers[4];

  b32 game_over;
  u32 score;
  u32 tick_counter;

  Vector2 move;
  u8 moving_to;
  b32 moved_down_last_tick;
  b32 aliens_are_moving;

  f64 time_coef;

  rect alien_rows[ALIENS_PER_COLUMN]; // For collision detection.
  // NOTE: this is a fix for when rows were not properly being deleted.
  b32 active_rows[ALIENS_PER_COLUMN];

  Bullet bullets[MAX_BULLETS]; // Array of bullets
  u16 player_active_bullets;   // Number of active bullets

  Bullet alien_bullets[16];
  u16 alien_active_bullets;
  u16 alien_should_shoot_counter;

  u8 num_of_lifes;

  b32 game_code_was_reloaded;

  b32 should_reset_aliens;
  b32 debug_mode;
  b32 debug_rectangles_mode;
  b32 debug_draw_aliens;
  rect debug_rect;
  Font debug_font;
  i32 debug_font_size;

  rect debug_full_rectangle;

  RayFunctions rl;
} GameApp;

#if 0
#ifdef GAME_DLL
#define DLL_CODE __declspec(dllexport)
#else
#define DLL_CODE __declspec(dllimport)
#endif
#endif
#define DLL_CODE

#define GAME_BEGIN(x) GameApp *x(int argc, char *argv[])
typedef GAME_BEGIN(game_begin_t);
DLL_CODE GAME_BEGIN(GameBegin);

#define GAME_END(x) void x(GameApp *game)
typedef GAME_END(game_end_t);
DLL_CODE GAME_END(GameEnd);

#define GAME_SIMULATE(x) void x(GameApp *game)
typedef GAME_SIMULATE(game_simulate_t);
DLL_CODE GAME_SIMULATE(GameSimulate);

#define GAME_DRAW(x) void x(GameApp *game)
typedef GAME_DRAW(game_draw_t);
DLL_CODE GAME_DRAW(GameDraw);

#define GAME_DRAW_DEBUG_INFO(x) void x(GameApp *game)
typedef GAME_DRAW_DEBUG_INFO(game_draw_debug_info_t);
DLL_CODE GAME_DRAW_DEBUG_INFO(GameDrawDebugInfo);

void Shoot(GameApp *game);
