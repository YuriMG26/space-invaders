#include "Game.h"
#include "utils.h"
#include <stdio.h>

// Loads texture safely, no matter what directory the application was launched.
internal Texture LoadTextureSafe(const char *path)
{
  Texture result;
  char texture_path[256];

  sprintf_s(texture_path, 256, "%s/%s", GetApplicationDirectory(), path);
  result = LoadTexture(texture_path);
  return result;
}

internal void LoadResources(GameApp *game)
{
  assert(game != NULL);
  game->graphics = LoadTextureSafe("assets/graphics.png");
  game->sound_assets.laser = LoadSound("assets/nsstudios__laser3.wav");
  game->sound_assets.explosion = LoadSound("assets/explosion.wav");
}

// Hard-coded function for getting the alien rectangles off of the graphics
// spritesheet.
internal Rectangle GetEnemyRectangle(GameApp *game, u8 alien_type)
{
  Rectangle result = {0};
  switch (alien_type)
  {
  case SHAPESHIFTER:
  case BLINKING_UFO:
  case OCTOPUS:
  {
    result.x = 5;
    result.y = 11;
    result.width = 8;
    result.height = 8;
  }
  break;
  case CRAB:
  {
    result.x = 22;
    result.y = 11;
    result.width = 11;
    result.height = 8;
  }
  break;
  case SQUID:
  {
    result.x = 5;
    result.y = 1;
    result.width = 8;
    result.height = 8;
  }
  break;
  case UFO:
  {
    result.x = 1;
    result.y = 40;
    result.width = 16;
    result.height = 7;
  }
  break;
    // case SHAPESHIFTER:
    // {
    //   // TODO
    // }
    // break;
    // case BLINKING_UFO:
    // {
    //   // TODO
    // }
    break;
  }
  return result;
}

ALWAYS_INLINE Rectangle GetPlayerRectangle(void)
{
  Rectangle result = {0};

  result.x = 3;
  result.y = 49;
  result.width = 13;
  result.height = 9;

  return result;
}

ALWAYS_INLINE void DrawPlayer(GameApp *game, Rectangle player_pos)
{
  DrawTexturePro(game->graphics, GetPlayerRectangle(), player_pos,
                 (Vector2){0, 0}, 0.0f, GREEN);
}

ALWAYS_INLINE void DrawAlien(GameApp *game, u8 alien_type, Rectangle alien_pos)
{
  DrawTexturePro(game->graphics, GetEnemyRectangle(game, alien_type), alien_pos,
                 (Vector2){0, 0}, 0.0f, WHITE);
}

void SimulateBullets(GameApp *game)
{
  for (u32 i = 0; i < MAX_BULLETS; ++i)
  {
    if (!game->bullets[i].active)
      continue;
  }
}

GameApp *GameBegin(int argc, char *argv[])
{
  // TODO: Parse Args
  GameApp *game = AllocAndZero(sizeof(GameApp));

  strcpy_s(game->title, 64, "Space Invaders");

  game->width = 1280;
  game->height = 960;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow((int)game->width, (int)game->height, game->title);
  SetTargetFPS(60);

  InitAudioDevice();

  LoadResources(game);

  u16 player_width = 60;
  u16 player_height = 30;
  game->player.pos =
      (Rectangle){Centralize(game->width, player_width),
                  GetScreenHeight() - 100, player_width, player_height};

  u8 current_alien_type = 1;
  u8 counter = 1;

  v2 rect_min = (v2){.x = 9999, .y = 9999};
  v2 rect_max = (v2){.x = 0, .y = 0};
  const u8 scaling_factor = 4;
  const u16 max_alien_width = 11;

  const u16 alien_jump_width = max_alien_width * (scaling_factor + 2);

  const u16 begin_x_position =
      Centralize(GetScreenWidth(),
                 ((ALIENS_PER_ROW - 1) * (alien_jump_width)) + 30) -
      1;
  u16 begin_x = begin_x_position;
  u16 begin_y = 70;

  for (u8 i = 0; i < ALIEN_NUM; ++i, ++counter)
  {
    if (counter == ALIENS_PER_ROW + 1)
    {
      game->alien_rows[current_alien_type - 1] =
          (Rectangle){rect_min.x, rect_min.y, rect_max.x - rect_min.x,
                      rect_max.y - rect_min.y};

      rect_min = (v2){.x = 9999, .y = 9999};
      rect_max = (v2){.x = 0, .y = 0};
      ++current_alien_type;
      counter = 1;
      begin_x = begin_x_position;
      begin_y += game->aliens[i - 1].pos.height + 20;
    }
    Rectangle aux = GetEnemyRectangle(game, current_alien_type);
    // u16 x = Centralize();
    // NOTE: Hard-coded 33.
    u16 x =
        begin_x +
        ((scaling_factor * max_alien_width) - (aux.width * scaling_factor)) / 2;
    game->aliens[i].pos = (Rectangle){x, begin_y, aux.width * scaling_factor,
                                      aux.height * scaling_factor};

    if (rect_min.x > x)
      rect_min.x = x;
    if (rect_min.y > begin_y)
      rect_min.y = begin_y;
    if (rect_max.x < x + aux.width * scaling_factor)
      rect_max.x = x + aux.width * scaling_factor;
    if (rect_max.y < begin_y + aux.height * scaling_factor)
      rect_max.y = begin_y + aux.height * scaling_factor;

    // TODO: generate min-max rectangles for when an alien is killed.
    begin_x += alien_jump_width;

    game->aliens[i].alive = true;
  }
  game->alien_rows[current_alien_type - 1] = (Rectangle){
      rect_min.x, rect_min.y, rect_max.x - rect_min.x, rect_max.y - rect_min.y};

  game->aliens_alive_count = ALIEN_NUM;

  return game;
}

internal void SwapBullets(Bullet *bullet_a, Bullet *bullet_b)
{
  Bullet temp;
  temp = *bullet_a;
  *bullet_a = *bullet_b;
  *bullet_b = temp;
}

internal void KillBullet(GameApp *game, Bullet *bullet)
{
  bullet->active = false;
  SwapBullets(bullet, &game->bullets[game->active_bullets - 1]);
  --game->active_bullets;
}

internal void CheckIfBulletISOutOfBounds(GameApp *game, Bullet *bullet, u16 i)
{
  if (bullet->pos.y - bullet->pos.height < -5)
  {
    KillBullet(game, bullet);
  }
}

void GameSimulate(GameApp *game)
{
  f32 delta = GetFrameTime();

  if (game->aliens_alive_count == 0)
  {
    const char *text = "YOU WON!";
    const u16 text_size = 35;
    DrawText(text, Centralize(GetScreenWidth(), MeasureText(text, text_size)),
             Centralize(GetScreenHeight(), text_size), text_size, WHITE);
    return;
  }

  const f32 player_velocity = 250.f;

  game->player.pos.x +=
      ((int)IsKeyDown(KEY_D) - (int)IsKeyDown(KEY_A)) * player_velocity * delta;

  if (IsKeyPressed(KEY_SPACE))
  {
    Shoot(game);
  }

  if (IsKeyPressed(KEY_F1))
  {
    game->debug_mode = !game->debug_mode;
  }

  for (u16 i = 0; i < game->active_bullets; ++i)
  {
    b32 bullet_collided = false;

    if (game->bullets[i].active)
    {
      for (u8 j = 0; j < ALIENS_PER_COLUMN && !bullet_collided; ++j)
      {
        if (CheckCollisionRecs(game->bullets[i].pos, game->alien_rows[j]))
        {
          for (u8 k = 0; k < ALIENS_PER_ROW && !bullet_collided; ++k)
          {
            Alien *alien = &game->aliens[_1d_index_to_2d(ALIENS_PER_ROW, j, k)];
            if (alien->alive == true &&
                CheckCollisionRecs(game->bullets[i].pos, alien->pos))
            {
              bullet_collided = true;
              KillBullet(game, &game->bullets[i]);
              alien->alive = false;
              game->aliens_alive_count--;
              PlaySound(game->sound_assets.explosion);
              break;
            }
          }
        }
      }

      CheckIfBulletISOutOfBounds(game, &game->bullets[i], i);
      game->bullets[i].pos.y -= 200 * delta;
    }
  }
}

void GameDraw(GameApp *game)
{
  u8 current_alien_type = 1;
  u8 counter = 1;
  for (u8 i = 0; i < ALIEN_NUM; ++i, ++counter)
  {
    if (counter == ALIENS_PER_ROW + 1)
    {
      ++current_alien_type;
      counter = 1;
    }
    char buffer[3] = {};

    if (game->aliens[i].alive == true)
      DrawAlien(game, current_alien_type, game->aliens[i].pos);

    // sprintf_s(buffer, 3, "%u", counter);
    // DrawText(buffer, x + 5, y + 5, 22, BLACK);
  }

  // DrawRectangleRounded(game->player.pos, 0.1, 0, GREEN);
  // Rectangle player_aim;
  // player_aim.height = 20;
  // player_aim.width = 10;
  // player_aim.x =
  //     Centralize(game->player.pos.width, player_aim.width) +
  //     game->player.pos.x;
  // player_aim.y = game->player.pos.y - 10;
  // DrawRectangleRounded(player_aim, 0.6f, 0, GREEN);
  //
  // DrawRectangleRounded(game->player.pos, 0.8, 0, GREEN);
  DrawPlayer(game, game->player.pos);

  for (u16 i = 0; i < MAX_BULLETS; ++i)
  {
    if (game->bullets[i].active)
    {
      DrawRectangleRec(game->bullets[i].pos, RED);
    }
  }
}

void GameEnd(GameApp *game)
{
  UnloadTexture(game->graphics);
  free(game);
  CloseWindow();
}

void GameDrawDebugInfo(GameApp *game)
{
  if (!game->debug_mode)
    return;
  const f32 roundness = 0.15f;
  const char *temp_buffer;
  char buffer[256] = {0};
  u16 begin_x = game->debug_rect.x + 15;
  u16 begin_y = game->debug_rect.y + 15;

  rect aux = game->debug_rect;
  aux.x -= 0.5;
  aux.y -= 0.5;
  aux.width += 1.5;
  aux.height += 1.5;

  DrawRectangleRounded(aux, roundness, 0, (Color){255, 255, 255, 90});
  DrawRectangleRounded(game->debug_rect, roundness, 0, WHITE);

  temp_buffer = "fps = %i";
  sprintf_s(buffer, 256, temp_buffer, GetFPS());
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  temp_buffer = "player.x = %.3f";
  sprintf_s(buffer, 256, temp_buffer, game->player.pos.x);
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  temp_buffer = "active_bullets = %u";
  sprintf_s(buffer, 256, temp_buffer, game->active_bullets);
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  temp_buffer = "distance = {%.2f %.2f}";
  sprintf_s(buffer, 256, temp_buffer, game->debug_full_rectangle.x - 0,
            GetScreenWidth() - (game->debug_full_rectangle.x +
                                game->debug_full_rectangle.width));
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  for (u16 i = 0; i < MAX_BULLETS; ++i)
  {
    if (game->bullets[i].active)
    {
      sprintf_s(buffer, 256, "%u = {%f, %f}", i, game->bullets[i].pos.x,
                game->bullets[i].pos.y);
      DrawTextEx(game->debug_font, buffer,
                 (Vector2){(float)begin_x, (float)begin_y},
                 game->debug_font_size, 0, DARKGRAY);
      begin_y += 25;
    }
  }

  for (u16 i = 0; i < ALIENS_PER_COLUMN; ++i)
  {
    DrawRectangleLinesEx(game->alien_rows[i], 1, GREEN);
  }

  // u8 current_alien_type = 1;
  u8 counter = 1;
  for (u8 i = 0; i < ALIEN_NUM; ++i, ++counter)
  {
    if (counter == ALIENS_PER_ROW + 1)
    {
      // ++current_alien_type;
      counter = 1;
    }
    f32 x = game->aliens[i].pos.x;
    f32 y = game->aliens[i].pos.y;
    DrawRectangleLines(x, y, game->aliens[i].pos.width,
                       game->aliens[i].pos.height,
                       (Color){200, 200, counter * 50, 255});
  }
}

void Shoot(GameApp *game)
{
  if (game->active_bullets == MAX_BULLETS)
    return;

  f32 width_height = 7;
  f32 x = Centralize(game->player.pos.width, width_height) + game->player.pos.x;
  f32 y = game->player.pos.y - width_height;

  game->bullets[game->active_bullets].active = true;
  game->bullets[game->active_bullets].pos =
      (Rectangle){x, y, width_height, width_height};

  game->active_bullets++;
  LogInfo(game->logger, "shooting bullet %u", game->active_bullets);

  PlaySound(game->sound_assets.laser);
}
