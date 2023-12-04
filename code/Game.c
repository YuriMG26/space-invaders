/*
 * TODO:
 * [] UFO
 * [] Correct SwapBullets bug
 */

#define GAME_CODE
#include "Game.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

i64 signal_counter = 0;
i64 zero_counter = 0;
b32 tick_signal = false;

void AudioProcess(void *buffer, unsigned int frames)
{
  i32 *d = (i32 *)buffer;

  for (unsigned int i = 0; i < frames; i++)
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
    else if (tick_signal)
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

// Loads texture safely, no matter what directory the application was launched.
internal Texture LoadTextureSafe(GameApp *game, const char *path)
{
  Texture result;
  char texture_path[256];

  sprintf_s(texture_path, 256, "%s/%s", GetApplicationDirectory(), path);
  result = LoadTexture(texture_path);
  return result;
}

internal Sound LoadSoundSafe(GameApp *game, const char *path)
{
  Sound result = {0};
  char sound_path[256];

  sprintf_s(sound_path, 256, "%s/%s", GetApplicationDirectory(), path);

  result = LoadSound(sound_path);
  return result;
}

#if 0
internal Wave LoadWaveSafe(GameApp *game, const char *path)
{
  Wave result = {0};
  char wave_path[256];

  sprintf_s(wave_path, 256, "%s/%s", GetApplicationDirectory(), path);

  result = LoadWave(wave_path);
  return result;
}
#endif

internal Music LoadMusicSafe(GameApp *game, const char *path)
{
  Music result = {0};
  char music_path[256];

  sprintf_s(music_path, 256, "%s/%s", GetApplicationDirectory(), path);

  result = LoadMusicStream(music_path);
  return result;
}

internal Font LoadFontSafe(GameApp *game, const char *path, int size)
{
  Font result = {0};
  char font_path[256];

  sprintf_s(font_path, 256, "%s/%s", GetApplicationDirectory(), path);
  result = LoadFontEx(font_path, size, 0, 0);
  return result;
}

internal void LoadResources(GameApp *game)
{
  assert(game != NULL);
  game->graphics = LoadTextureSafe(game, "assets/graphics.png");
  game->sound_assets.laser =
      LoadSoundSafe(game, "assets/nsstudios__laser3.wav");
  game->sound_assets.explosion = LoadSoundSafe(game, "assets/explosion.wav");
  game->sound_assets.explosion_barrier =
      LoadSoundSafe(game, "assets/explosion.wav");
  game->sound_assets.background = LoadMusicSafe(game, "assets/background2.wav");
  game->sound_assets.background.looping = true;
  SetSoundVolume(game->sound_assets.explosion, 0.5f);
}

internal void UpdateScore(GameApp *game, u8 alien_type)
{
  switch (alien_type)
  {
  case OCTOPUS:
    game->score += 10;
    break;
  case CRAB:
    game->score += 20;
    break;
  case SQUID:
    game->score += 30;
    break;
  case UFO:
    game->score += 40;
    break;
  default:
    game->score += 420;
    break;
  }
}

internal void CheckPlayerWon(GameApp *game)
{

  if (game->aliens_alive_count == 0)
  {
    const char *text = "YOU WON!";
    const u16 text_size = 35;
    DrawText(text, Centralize(GetScreenWidth(), MeasureText(text, text_size)),
             Centralize(GetScreenHeight(), text_size), text_size, WHITE);
    return;
  }
}

ALWAYS_INLINE void UpdateInputValues(KeyInput *input, bool left, bool right,
                                     bool shoot, bool gamepad_mode)
{
  input->gamepad_mode = gamepad_mode;
  input->left = left;
  input->right = right;
  input->shoot = shoot;
}

internal void UpdatePlayerControl(GameApp *game)
{
  game->input.shoot = false;
  game->input.left = false;
  game->input.right = false;
  if (game->player_dead)
    return;
  if (IsGamepadAvailable(0))
  {
    bool left = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
    bool right = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
    bool shoot = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    if (left || right || shoot)
    {
      UpdateInputValues(&game->input, left, right, shoot, true);
    }
  }
  bool left = IsKeyDown(KEY_A);
  bool right = IsKeyDown(KEY_D);
  bool shoot = IsKeyPressed(KEY_SPACE);
  if (left || right || shoot)
  {
    UpdateInputValues(&game->input, left, right, shoot, false);
  }
}

internal void SimulatePlayer(GameApp *game)
{
  const f32 player_velocity = 250.f;
  UpdatePlayerControl(game);
  game->player.pos.x +=
      (game->input.right - game->input.left) * player_velocity * game->delta;
  game->player.hitbox.x = game->player.pos.x;
  if (game->input.shoot)
  {
    Shoot(game);
  }
}

internal bool CheckMusicTick(GameApp *game)
{
  bool result = false;
  if (tick_signal)
  {
    game->tick_counter++;
    if (game->tick_counter == 1)
    {
      result = true;
    }
  }
  else
  {
    game->tick_counter = 0;
  }
  return result;
}

internal void SwapBullets(Bullet *bullet_a, Bullet *bullet_b)
{
  // TODO/NOTE: there is a bug when the player is shooting the barriers.
  Bullet temp;
  temp = *bullet_a;
  *bullet_a = *bullet_b;
  *bullet_b = temp;
}

internal void KillPlayerBullet(GameApp *game, Bullet *bullet)
{
  bullet->active = false;
  SwapBullets(bullet, &game->player_bullets[game->player_active_bullets - 1]);
  --game->player_active_bullets;
}

internal void CheckCollisionBulletBarrier(GameApp *game, u8 bullet_index,
                                          u8 barrier_index)
{
  Bullet *bullet = &game->player_bullets[bullet_index];
  Barrier *barrier = &game->barriers[barrier_index];

  for (u8 i = 0; i < BARRIER_RECTANGLES_AMMOUNT; ++i)
  {
    if (barrier->active_rectangles[i] == true &&
        CheckCollisionRecs(bullet->pos, barrier->rectangles[i]))
    {
      // TODO: damage
      barrier->active_rectangles[i] = false;
      KillPlayerBullet(game, bullet);
      PlaySound(game->sound_assets.explosion_barrier);
    }
  }
}

internal void CheckDebugKeys(GameApp *game)
{
#if RELEASE_BUILD == 0
  if (IsKeyPressed(KEY_F1))
    game->debug_mode = !game->debug_mode;

  if (IsKeyPressed(KEY_F2))
    game->draw_hitboxes = !game->draw_hitboxes;

  if (IsKeyPressed(KEY_F3))
    game->debug_draw_aliens = !game->debug_draw_aliens;

  if (IsKeyPressed(KEY_F4))
    game->aliens_are_moving = !game->aliens_are_moving;
#endif
}

internal void SimulatePlayerBullets(GameApp *game)
{
  for (u16 i = 0; i < game->player_active_bullets; ++i)
  {
    b32 bullet_collided = false;

    if (game->player_bullets[i].active)
    {
      const f32 default_barrier_y = game->barriers[0].pos.y;
      if (game->player_bullets[i].pos.y > default_barrier_y)
      {
        for (u8 j = 0; j < 4; ++j)
        {
          CheckCollisionBulletBarrier(game, i, j);
        }
      }
      else
      {
        for (u8 j = 0; j < ALIENS_PER_COLUMN && !bullet_collided; ++j)
        {
          if (CheckCollisionRecs(game->player_bullets[i].pos,
                                 game->alien_rows[j]))
          {
            for (u8 k = 0; k < ALIENS_PER_ROW && !bullet_collided; ++k)
            {
              Alien *alien =
                  &game->aliens[_1d_index_to_2d(ALIENS_PER_ROW, j, k)];
              if (alien->alive == true &&
                  CheckCollisionRecs(game->player_bullets[i].pos, alien->pos))
              {
                bullet_collided = true;
                KillPlayerBullet(game, &game->player_bullets[i]);
                alien->alive = false;
                UpdateScore(game, alien->type);
                game->aliens_alive_count--;
                PlaySound(game->sound_assets.explosion);
                UpdateRowRectangle(game, j, &game->alien_rows[j]);
                break;
              }
            }
          }
        }
      }

      CheckIfBulletISOutOfBounds(game, &game->player_bullets[i], i);
      game->player_bullets[i].pos.y -= 500 * game->delta;
    }
  }
}

inline f32 GetBottomYCoordinateFromYPoint(f32 ypoint, f32 height)
{
  return (ypoint + height);
}

inline f32 GetXCoordinateFromRightPoint(f32 right_point, f32 width)
{
  return (right_point - width);
}

internal b32 GetFirstAlienIndex(GameApp *game, u8 row_num, u8 *index)
{
  Alien *alien_array =
      &game->aliens[_1d_index_to_2d(ALIENS_PER_ROW, row_num, 0)];
  b32 alien_was_found = false;
  for (u8 i = 0; i < ALIENS_PER_ROW; ++i)
  {
    if (alien_array[i].alive == true)
    {
      *index = i;
      alien_was_found = true;
      break;
    }
  }
  return alien_was_found;
}

internal b32 GetLastAlienIndex(GameApp *game, u8 row_num, u8 *index)
{
  Alien *alien_array =
      &game->aliens[_1d_index_to_2d(ALIENS_PER_ROW, row_num, 0)];
  b32 alien_was_found = false;
  for (u8 i = ALIENS_PER_ROW - 1; i >= 0; --i)
  {
    if (alien_array[i].alive == true)
    {
      *index = i;
      alien_was_found = true;
      break;
    }
  }
  return alien_was_found;
}

// TODO: generic function for getting graphics off of the spritesheet?
internal rect GetMiscGraphics(GameApp *game, u8 misc_type, u8 modifier)
{
  rect result;
  switch (misc_type)
  {
  case ALIEN_BULLET:
    assert(modifier < 4);
    result = (rect){1 + (modifier * 5), 21, 3, 7};
    break;
  }
  return result;
}

internal void AnimateBullets(GameApp *game)
{
  static f64 last_animation_tick;
  // TODO: really use GetTime() or is it better converting the current frame
  // time to ticks and calculate animation time on that? Hmmmm

  // NOTE: the entire cycle should be done in 9 frames on 60hz, thus 18 frames
  // on 120hz and so forth.
  const f64 total_cycle_duration = 9.0 / 60.0;
  const f64 bullet_animation_interval = total_cycle_duration / 3;
  f64 time = GetTime();

  if (last_animation_tick + bullet_animation_interval < time)
  {
    last_animation_tick = time;
    game->alien_bullet_animation_state =
        (game->alien_bullet_animation_state + 1 == 4)
            ? 0
            : game->alien_bullet_animation_state + 1;
  }
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
    result = (game->animation_state == ANIMATION_ZERO)
                 ? (Rectangle){.x = 39, .y = 11, .width = 12, .height = 8}
                 : (Rectangle){.x = 39, .y = 1, .width = 12, .height = 8};
  }
  break;
  case CRAB:
  {
    result = (game->animation_state == ANIMATION_ZERO)
                 ? (Rectangle){.x = 22, .y = 11, .width = 11, .height = 8}
                 : (Rectangle){.x = 22, .y = 1, .width = 11, .height = 8};
  }
  break;
  case SQUID:
  {
    result = (game->animation_state == ANIMATION_ZERO)
                 ? (Rectangle){.x = 5, .y = 1, .width = 8, .height = 8}
                 : (Rectangle){.x = 5, .y = 11, .width = 8, .height = 8};
  }
  break;
  case UFO:
  {
    result = (Rectangle){.x = 1, .y = 40, .width = 16, .height = 7};
  }
  break;
  }
  return result;
}

inline Rectangle GetBarrierRectangle(void)
{
  Rectangle result = {0};

  result.x = 46;
  result.y = 31;
  result.width = 22;
  result.height = 16;

  return result;
}

inline Rectangle GetPlayerRectangle(void)
{
  Rectangle result = {0};

  result.x = 2;
  result.y = 49;
  result.width = 15;
  result.height = 8;

  return result;
}

internal Rectangle GetPlayerDeathAnimation(u8 frame)
{
  Rectangle result = {0};
  switch (frame)
  {
  case 0:
    result.x = 20;
    result.y = 49;
    result.width = 15;
    result.height = 8;
    break;
  case 1:
    result.x = 37;
    result.y = 49;
    result.width = 15;
    result.height = 8;
    break;
  case 2:
    result.x = 0;
    result.y = 0;
    result.width = 1;
    result.height = 1;
    break;
  }
  return result;
}

// TODO: animation system please
internal void AnimatePlayerDeath(GameApp *game)
{
  static u8 animation_phase;
  static f64 death_started_time;
  static bool death_started;

  static f64 static_phase_started_time;
  static bool static_phase_started;

  if (!death_started)
  {
    death_started = true;
    death_started_time = GetTime();
  }

  f64 time_now = GetTime();
  if (animation_phase == FLICKERING)
  {
    const f64 total_flickering_animation_time = 1.0; // in seconds
    const f64 single_flicker_time = total_flickering_animation_time /
                                    (total_flickering_animation_time * 10);
    static f64 last_flicker_time = 1.0 / (1.0 * 10);
    static u8 flicker_frame = 3;

    if (time_now > death_started_time + total_flickering_animation_time)
    {
      animation_phase = FINAL_STATIC;
    }

    // check if it is time to flicker
    if (last_flicker_time + single_flicker_time < time_now)
    {
      // if so, flicker.
      flicker_frame = (flicker_frame == 0) ? 1 : 0;
      game->player.animation = GetPlayerDeathAnimation(flicker_frame);
      last_flicker_time = time_now;
    }
  }
  else if (animation_phase == FINAL_STATIC)
  {
    game->player.animation = GetPlayerDeathAnimation(2);
    if (!static_phase_started)
    {
      static_phase_started = true;
      static_phase_started_time = GetTime();
    }
    const f64 total_static_time = 2.0;
    if (time_now > total_static_time + static_phase_started_time)
    {
      if (game->game_over == false)
      {
        game->player_dead = false;
        game->aliens_are_moving = true;
      }
      animation_phase = 0;
      death_started = false;
      static_phase_started = false;
      game->player.pos.x = Centralize(GetScreenWidth(), game->player.pos.width);
      game->player_death_animation_just_ended = true;

      ResumeMusicStream(game->sound_assets.background);
    }
  }
}

internal void CreateFullRectangle(GameApp *game)
{
  v2 rect_min = (v2){.x = 9999, .y = 9999};
  v2 rect_max = (v2){.x = 0, .y = 0};
  for (u8 i = 0; i < ALIENS_PER_COLUMN; ++i)
  {
    if (game->active_rows[i] == false)
      continue;
    rect *current_row = &game->alien_rows[i];
    rect_min.x = (current_row->x < rect_min.x) ? current_row->x : rect_min.x;
    rect_min.y = (current_row->y < rect_min.y) ? current_row->y : rect_min.y;

    rect_max.x = (current_row->x + current_row->width > rect_max.x)
                     ? current_row->x + current_row->width
                     : rect_max.x;
    rect_max.y = (current_row->y + current_row->height > rect_max.y)
                     ? current_row->y + current_row->height
                     : rect_max.y;
  }

  game->debug_full_rectangle = (Rectangle){
      rect_min.x, rect_min.y, rect_max.x - rect_min.x, rect_max.y - rect_min.y};
}

inline void DrawPlayer(GameApp *game, Rectangle player_pos)
{
  DrawTexturePro(game->graphics, game->player.animation, player_pos,
                 (Vector2){0, 0}, 0.0f, GREEN);
}

inline void DrawAlien(GameApp *game, u8 alien_type, Rectangle alien_pos,
                      Color tint)
{
  DrawTexturePro(game->graphics, GetEnemyRectangle(game, alien_type), alien_pos,
                 (Vector2){0, 0}, 0.0f, tint);
}

internal void CheckPlayerLifes(GameApp *game)
{
  if (game->num_of_lifes == 0)
  {
    game->game_over = true;
  }
}

internal void CheckIfBulletISOutOfBounds(GameApp *game, Bullet *bullet, u16 i)
{
  if (bullet->pos.y - bullet->pos.height < -5)
  {
    KillPlayerBullet(game, bullet);
  }
}

internal void KillAlienBullet(GameApp *game, u32 bullet_index)
{
  assert(bullet_index <= MAX_ALIEN_BULLETS && bullet_index >= 0);
  LogInfo(NULL, "killing alien bullet %u", bullet_index);
  game->alien_bullets[bullet_index].active = false;
  DEBUG_ASSERT(game->alien_active_bullets == 0);
  SwapBullets(&game->alien_bullets[bullet_index],
              &game->alien_bullets[game->alien_active_bullets - 1]);
  --game->alien_active_bullets;
  LogInfo(NULL, "Killing bullet %u", bullet_index);
}

internal void AlienShoot(GameApp *game, u32 alien_index)
{
  if (game->alien_active_bullets == MAX_ALIEN_BULLETS)
  {
    return;
  }

  f32 scale = 3.5;

  Vector2 bullet_size = {3 * scale, 11 * scale};
  rect *alien_pos = &game->aliens[alien_index].pos;
  f32 x = Centralize(alien_pos->width, bullet_size.x) + alien_pos->x;
  f32 y = alien_pos->y + alien_pos->height + 2;
  game->alien_bullets[game->alien_active_bullets].active = true;
  rect rectangle = (Rectangle){x, y, bullet_size.x, bullet_size.y};
  game->alien_bullets[game->alien_active_bullets].pos = rectangle;

  // DEBUG_ASSERT(y < 1200);

  LogInfo(NULL, "alien %u shooting bullet %u", alien_index,
          game->alien_active_bullets);

  game->alien_active_bullets++;

  PlaySound(game->sound_assets.laser);
}

internal void UpdateAlienPositions(GameApp *game)
{
#define DEFAULT_MOVE 17

  if (game->moving_to == LEFT)
  {
    game->move = (Vector2){-DEFAULT_MOVE, 0};
    game->moved_down_last_tick = false;
    game->moving_to = LEFT;
  }
  else if (game->moving_to == RIGHT)
  {
    game->move = (Vector2){DEFAULT_MOVE, 0};
    game->moved_down_last_tick = false;
    game->moving_to = RIGHT;
  }

  if (!game->moved_down_last_tick)
  {
    rect full_rectangle = (Rectangle){
        game->debug_full_rectangle.x + game->move.x,
        game->debug_full_rectangle.y + game->move.y,
        game->debug_full_rectangle.width, game->debug_full_rectangle.height};

    if (full_rectangle.x < 0 ||
        full_rectangle.width + full_rectangle.x > GetScreenWidth())
    {
      game->move = (Vector2){0, DEFAULT_MOVE};
      game->moved_down_last_tick = true;
      game->moving_to = (game->moving_to == LEFT) ? RIGHT : LEFT;

      const f32 barrier_bottom =
          game->barriers[0].pos.y + game->barriers[0].pos.height;
      const f32 rect_bottom = GetBottomYCoordinateFromYPoint(
          game->debug_full_rectangle.y, game->debug_full_rectangle.height);
      if (rect_bottom + game->move.y >= barrier_bottom)
      {
        game->game_over = true;
        return;
      }
    }
  }

  game->debug_full_rectangle.x += game->move.x;
  game->debug_full_rectangle.y += game->move.y;
  for (u8 i = 0; i < ALIEN_NUM; ++i)
  {
    game->aliens[i].pos.x += game->move.x;
    game->aliens[i].pos.y += game->move.y;
  }

  for (u8 i = 0; i < ALIENS_PER_COLUMN; ++i)
  {
    game->alien_rows[i].x += game->move.x;
    game->alien_rows[i].y += game->move.y;
  }
}

internal void InitBarriersHitboxes(GameApp *game, Barrier *barrier)
{
  assert(barrier != NULL);
  const f32 box_width = barrier->pos.width / BARRIERS_COLS;
  const f32 box_height = barrier->pos.height / BARRIERS_ROWS;

  f32 current_x = barrier->pos.x;
  f32 current_y = barrier->pos.y;

  for (u8 i = 0; i < BARRIER_RECTANGLES_AMMOUNT; ++i)
  {
    barrier->rectangles[i].width = box_width;
    barrier->rectangles[i].height = box_height;
    barrier->active_rectangles[i] = true;
  }
  u8 i = 0;
  // TODO: compress?
  for (i = 0; i < (BARRIER_RECTANGLES_AMMOUNT / BARRIERS_ROWS); ++i)
  {
    barrier->rectangles[i].x = current_x;
    barrier->rectangles[i].y = current_y;
    current_x += box_width;
  }
  current_x = barrier->pos.x;
  current_y += box_height;
  for (; i < (BARRIER_RECTANGLES_AMMOUNT / BARRIERS_ROWS) * 2; ++i)
  {
    barrier->rectangles[i].x = current_x;
    barrier->rectangles[i].y = current_y;
    current_x += box_width;
  }
  current_x = barrier->pos.x;
  current_y += box_height;
  for (; i < (BARRIER_RECTANGLES_AMMOUNT / BARRIERS_ROWS) * 3; ++i)
  {
    barrier->rectangles[i].x = current_x;
    barrier->rectangles[i].y = current_y;
    current_x += box_width;
  }
}

internal void SetBarriersPositions(GameApp *game)
{
  const f32 default_padding = 210;
  const Rectangle default_rectangle = GetBarrierRectangle();
  const f32 default_width = default_rectangle.width * 5;
  const f32 default_height = default_rectangle.height * 3;
  const f32 default_ypos = GetScreenHeight() - 250;
  const u8 default_ammount_of_barriers = 4;

  f32 right_padding = GetScreenWidth() - default_padding;
  f32 left_padding = default_padding;

  Rectangle aux_rect =
      (Rectangle){left_padding, default_ypos, default_width, default_height};

  Barrier left_barrier;
  left_barrier.pos = aux_rect;

  Barrier right_barrier;
  aux_rect.x = GetXCoordinateFromRightPoint(right_padding, aux_rect.width);
  right_barrier.pos = aux_rect;

  game->barriers[0] = left_barrier;
  InitBarriersHitboxes(game, &game->barriers[0]);
  game->barriers[3] = right_barrier;
  InitBarriersHitboxes(game, &game->barriers[3]);

  f32 distance_between_barriers =
      game->barriers[3].pos.x -
      (game->barriers[0].pos.x + game->barriers[0].pos.width);

  f32 padding_between_barriers =
      (distance_between_barriers - (game->barriers[0].pos.width * 2)) /
      (default_ammount_of_barriers - 1);

  rect aux = game->barriers[0].pos;

  Barrier center_left_barrier;
  aux.x = aux.x + aux.width + padding_between_barriers;
  center_left_barrier.pos = aux;
  game->barriers[1] = center_left_barrier;
  InitBarriersHitboxes(game, &game->barriers[1]);

  Barrier center_right_barrier;
  aux.x = aux.x + aux.width + padding_between_barriers;
  center_right_barrier.pos = aux;
  game->barriers[2] = center_right_barrier;
  InitBarriersHitboxes(game, &game->barriers[2]);
}

internal Rectangle GenerateRowRectangle(Alien first, Alien last)
{
  v2 min = (v2){.x = first.pos.x, .y = first.pos.y};
  v2 max =
      (v2){.x = last.pos.x + last.pos.width, .y = last.pos.y + last.pos.height};

  Rectangle outer = (Rectangle){min.x, min.y, max.x - min.x, max.y - min.y};

  return outer;
}

internal void SetAliensPositions(GameApp *game)
{
  u8 current_alien_type = 1;
  u8 counter = 0;
  const u8 scaling_factor = 4;
  const u16 max_alien_width = 11;

  const u16 alien_jump_width = max_alien_width * (scaling_factor + 2);

  const u16 begin_x_position =
      Centralize((f32)GetScreenWidth(),
                 ((ALIENS_PER_ROW - 1) * (alien_jump_width)) + 30) -
      1;
  u16 begin_x = begin_x_position;
  u16 begin_y = 70;

  current_alien_type = 0;
  for (u8 i = 0; i < ALIENS_PER_COLUMN; ++i)
  {
    if (i == 0)
      current_alien_type = SQUID;
    else if (i >= 1 && i < 3)
      current_alien_type = CRAB;
    else if (i >= 3 && i < ALIENS_PER_COLUMN)
      current_alien_type = OCTOPUS;
    else
      current_alien_type = UFO;

    begin_x = begin_x_position;
    begin_y += 45;

    u8 start_index = 0, end_index = 255;

    Alien *alien_row = &game->aliens[_1d_index_to_2d(ALIENS_PER_ROW, i, 0)];
    if (game->should_reset_aliens)
    {

      for (u8 j = 0; j < ALIENS_PER_ROW; ++j)
      {
        Alien *current_alien =
            &game->aliens[_1d_index_to_2d(ALIENS_PER_ROW, i, j)];

        current_alien->type = current_alien_type;
        Rectangle aux = GetEnemyRectangle(game, current_alien_type);
        u16 x = begin_x + ((scaling_factor * max_alien_width) -
                           (aux.width * scaling_factor)) /
                              2;
        current_alien->pos = (Rectangle){x, begin_y, aux.width * scaling_factor,
                                         aux.height * scaling_factor};
        begin_x += alien_jump_width;

        current_alien->alive = true;
        current_alien->id = counter++;
      }
      start_index = 0;
      end_index = 10;
    }
    else
    {
      GetFirstAlienIndex(game, i, &start_index);
      GetLastAlienIndex(game, i, &end_index);
    }

    Rectangle outer =
        GenerateRowRectangle(alien_row[start_index], alien_row[end_index]);
    game->alien_rows[i] = outer;
  }
  CreateFullRectangle(game);

  for (u8 i = 0; i < ALIENS_PER_COLUMN; ++i)
  {
    for (u8 j = 0; j < ALIENS_PER_ROW; ++j)
    {
      Alien *current_alien =
          &game->aliens[_1d_index_to_2d(ALIENS_PER_ROW, i, j)];
      if (current_alien->alive == false)
      {
        // LogError(game->logger, "alien (%u, %u) is not alive!", i, j);
      }
    }
  }

  CreateFullRectangle(game);

  game->aliens_alive_count = ALIEN_NUM;
  game->should_reset_aliens = false;
}

GameApp *GameBegin(int argc, char *argv[])
{
  // TODO: Parse Args
  GameApp *game = AllocAndZero(sizeof(GameApp));
  game->should_reset_aliens = true;

  strcpy_s(game->title, 64, "Space Invaders");

  game->width = 1280;
  game->height = 960;

  game->num_of_lifes = 3;
  game->moving_to = RIGHT;
  game->should_animate_player_death = true;
  game->moved_down_last_tick = true;
  game->aliens_are_moving = true;
  game->paused = false;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow((int)game->width, (int)game->height, game->title);

#if RELEASE_BUILD == 0
  game->debug_mode = true;
  game->draw_hitboxes = false;
#endif
  game->debug_draw_aliens = true;
  game->debug_rect = (Rectangle){10, 10, 350, 550};
  game->debug_font_size = 24;
  game->debug_font =
      LoadFontSafe(game, "assets/inconsolata_bold.ttf", game->debug_font_size);

  i32 refresh_rate = GetMonitorRefreshRate(GetCurrentMonitor());
  if (refresh_rate == 0)
    refresh_rate = 60;
  SetTargetFPS(refresh_rate);

  InitAudioDevice();

  LoadResources(game);

  SetMasterVolume(0.8f);
  SetSoundVolume(game->sound_assets.explosion, 0.15f);
  SetSoundPitch(game->sound_assets.explosion_barrier, 1.4f);
  SetSoundVolume(game->sound_assets.explosion_barrier, 0.09f);
  SetSoundVolume(game->sound_assets.laser, 0.4f);

  u16 player_width = 60;
  u16 player_height = 30;
  game->player.pos =
      (Rectangle){Centralize(game->width, player_width),
                  GetScreenHeight() - 100, player_width, player_height};
  game->player.hitbox = game->player.pos;
  game->player.hitbox.y += 10;
  game->player.hitbox.height -= 12;

  for (u8 i = 0; i < ALIENS_PER_COLUMN; ++i)
  {
    game->active_rows[i] = true;
  }

  SetAliensPositions(game);

  while (!IsMusicReady(game->sound_assets.background))
    ;

  PlayMusicStream(game->sound_assets.background);
  AttachAudioStreamProcessor(game->sound_assets.background.stream,
                             AudioProcess);

  SetBarriersPositions(game);

  return game;
}

internal void UpdateRowRectangle(GameApp *game, u8 row_num, rect *alien_row)
{
  if (game->active_rows[row_num] == false)
    return;

  // Getting first alive alien
  Alien *alien_array =
      &game->aliens[_1d_index_to_2d(ALIENS_PER_ROW, row_num, 0)];
  u8 start_index = 0, end_index = 255;

  if (!GetFirstAlienIndex(game, row_num, &start_index))
  {
    game->active_rows[row_num] = false;
    *alien_row = (Rectangle){.x = 0, .y = 0, .width = 0, .height = 0};
    CreateFullRectangle(game);
    return;
  }

  if (!GetLastAlienIndex(game, row_num, &end_index))
  {
    game->active_rows[row_num] = false;
    *alien_row = (Rectangle){.x = 0, .y = 0, .width = 0, .height = 0};
    CreateFullRectangle(game);
    return;
  }

  u16 diff = end_index - start_index;

  if (diff + 1 == 11)
    return;

  Rectangle outer =
      GenerateRowRectangle(alien_array[start_index], alien_array[end_index]);

  *alien_row = outer;

  assert(outer.width != 0);
  assert(outer.height != 0);

  CreateFullRectangle(game);
}

internal void CheckAlienBulletsCollison(GameApp *game)
{
  assert(game->alien_active_bullets <= MAX_ALIEN_BULLETS);
  for (u8 i = 0; i < game->alien_active_bullets; ++i)
  {
    if (game->alien_bullets[i].active)
    {
      // Check collision with player
      if (CheckCollisionRecs(game->alien_bullets[i].pos, game->player.hitbox))
      {
        KillAlienBullet(game, i);
        game->num_of_lifes--;
        PlaySound(game->sound_assets.explosion);
        game->player_dead = true;
        game->player_dead_time = GetTime();
        continue;
      }
      for (u8 j = 0; j < 4; ++j)
      {
        if (CheckCollisionRecs(game->alien_bullets[i].pos,
                               game->barriers[j].pos))
        {
          Barrier *barrier = &game->barriers[j];
          for (u8 k = 0; k < BARRIER_RECTANGLES_AMMOUNT; ++k)
          {
            if (game->alien_bullets[i].active &&
                game->barriers[j].active_rectangles[k] == true &&
                CheckCollisionRecs(game->alien_bullets[i].pos,
                                   barrier->rectangles[k]))
            {
              LogInfo(NULL, "\tbullet %u colliding with hitbox %u", i, k);
              barrier->active_rectangles[k] = false;
              KillAlienBullet(game, i);
              PlaySound(game->sound_assets.explosion_barrier);
            }
          }
        }
      }
      // Check collision with bottom of the screen
      if (game->alien_bullets[i].pos.y > GetScreenHeight())
        KillAlienBullet(game, i);
      const f32 alien_bullet_speed = 500;
      game->alien_bullets[i].pos.y += alien_bullet_speed * game->delta;
    }
  }
}

void GameSimulate(GameApp *game)
{
  if (game->player_death_animation_just_ended == true)
    game->player_death_animation_just_ended = false;
  game->delta = GetFrameTime();
  CheckPlayerLifes(game);

  game->time_coef = (double)(56 - game->aliens_alive_count) / (double)ALIEN_NUM;
  UpdateMusicStream(game->sound_assets.background);

  if (game->player_dead == true)
  {
    game->aliens_are_moving = false;
    if (IsMusicStreamPlaying(game->sound_assets.background))
    {
      PauseMusicStream(game->sound_assets.background);
    }
    if (game->should_animate_player_death)
      AnimatePlayerDeath(game);
  }
  else
  {
    game->player.animation = GetPlayerRectangle();
  }

  if (game->game_over)
  {
    if (game->player_death_animation_just_ended)
      game->should_animate_player_death = false;
    const char *text = "GAME OVER";
    const u16 text_size = 35;
    DrawText(text, Centralize(GetScreenWidth(), MeasureText(text, text_size)),
             Centralize(GetScreenHeight(), text_size), text_size, WHITE);
    return;
  }

  CheckPlayerWon(game);
  SimulatePlayer(game);
  CheckDebugKeys(game);
  SimulatePlayerBullets(game);

  Vector2 mouse_pos = GetMousePosition();
  if (CheckCollisionPointRec(mouse_pos, game->debug_rect) &&
      IsMouseButtonDown(MOUSE_BUTTON_LEFT))
  {
    Vector2 mouse_delta = GetMouseDelta();
    game->debug_rect.x += mouse_delta.x;
    game->debug_rect.y += mouse_delta.y;
  }

  CheckAlienBulletsCollison(game);

  i32 random_alien = 0;
  while (game->aliens_alive_count > 0)
  {
    random_alien = GetRandomValue(0, 54);
    if (game->aliens[random_alien].alive == true)
      break;
  }

  if (!game->paused && CheckMusicTick(game) && game->aliens_are_moving)
  {
    game->animation_state = (game->animation_state == ANIMATION_ZERO)
                                ? ANIMATION_ONE
                                : ANIMATION_ZERO;
    UpdateAlienPositions(game);
    SetMusicPitch(game->sound_assets.background, 1.0 + game->time_coef);
    if (++game->alien_should_shoot_counter == 4 && game->aliens_alive_count > 0)
    {
      AlienShoot(game, random_alien);
      game->alien_should_shoot_counter = 0;
    }
    PlayMusicStream(game->sound_assets.background);
  }
}

void GameDraw(GameApp *game)
{
  for (u8 i = 0; i < ALIEN_NUM; ++i)
  {
    if (game->aliens[i].alive == true)
    {
      if (game->draw_hitboxes)
        DrawRectangleLinesEx(game->aliens[i].pos, 1.0f, RED);
      if (game->debug_draw_aliens)
      {
        Color c;
        c = WHITE;

        DrawAlien(game, game->aliens[i].type, game->aliens[i].pos, c);
      }
    }
  }

  for (u8 i = 0; i < 4; ++i)
  {
    Color c = GREEN;
    DrawTexturePro(game->graphics, GetBarrierRectangle(), game->barriers[i].pos,
                   (Vector2){0, 0}, 0.0, c);

    for (u8 j = 0; j < BARRIER_RECTANGLES_AMMOUNT; ++j)
    {
      if (game->barriers[i].active_rectangles[j])
      {
        if (game->draw_hitboxes)
          DrawRectangleLinesEx(game->barriers[i].rectangles[j], 1.0f, RED);
      }
      else
        DrawRectangleRec(game->barriers[i].rectangles[j], BLACK);
    }
  }

  if (game->draw_hitboxes)
    DrawRectangleLinesEx(game->player.hitbox, 1.0f, RED);
  DrawPlayer(game, game->player.pos);

  for (u16 i = 0; i < game->player_active_bullets; ++i)
  {
    if (game->player_bullets[i].active)
      DrawRectangleRec(game->player_bullets[i].pos, WHITE);
  }

  for (u16 i = 0; i < game->alien_active_bullets; ++i)
  {
    if (game->alien_bullets[i].active)
    {
      // TODO: generic function for animation
      AnimateBullets(game);
      DrawTexturePro(game->graphics,
                     GetMiscGraphics(game, ALIEN_BULLET,
                                     game->alien_bullet_animation_state),
                     game->alien_bullets[i].pos, (Vector2){0, 0}, 0.0, WHITE);
    }
  }

  Rectangle green_bottom =
      (Rectangle){0, GetScreenHeight() - 10, GetScreenWidth(), 10};
  DrawRectangleRec(green_bottom, GREEN);

  GameDrawDebugInfo(game);

  char score_buffer[64] = {0};
  sprintf_s(score_buffer, 64, "SCORE: %u", game->score);
  DrawText(score_buffer, 30, 20, 40, WHITE);
  const char *text = "LIVES";
  u16 lives_text_x = (GetScreenWidth() / 2) + 100;
  DrawText(text, lives_text_x, 20, 40, WHITE);

  // TODO: measuretext
  f32 life_rect_x = (lives_text_x) + 150;
  Rectangle life_rect;

  for (u8 i = 0; i < game->num_of_lifes; ++i)
  {
    life_rect = (Rectangle){life_rect_x, 20, game->player.pos.width,
                            game->player.pos.height};
    DrawTexturePro(game->graphics, GetPlayerRectangle(), life_rect,
                   (Vector2){0, 0}, 0.0, GREEN);
    life_rect_x += game->player.pos.width + 10;
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
  i16 begin_x = game->debug_rect.x + 15;
  i16 begin_y = game->debug_rect.y + 15;

  rect aux = game->debug_rect;
  aux.x -= 0.5;
  aux.y -= 0.5;
  aux.width += 1.5;
  aux.height += 1.5;

  DrawRectangleRounded(aux, roundness, 0, (Color){255, 255, 255, 90});
  DrawRectangleRounded(game->debug_rect, roundness, 0, WHITE);

  temp_buffer = "input_mode: %s";
  sprintf_s(buffer, 256, temp_buffer,
            (game->input.gamepad_mode) ? "GAMEPAD" : "KEYBOARD");
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

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

  temp_buffer = "player_active_bullets = %u";
  sprintf_s(buffer, 256, temp_buffer, game->player_active_bullets);
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  temp_buffer = "time_coef: %lf";
  sprintf_s(buffer, 256, temp_buffer, game->time_coef);
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  temp_buffer = "moving_to = %s";
  sprintf_s(buffer, 256, temp_buffer,
            (game->moving_to == LEFT) ? "LEFT" : "RIGHT");
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  temp_buffer = "moved_down = %s";
  sprintf_s(buffer, 256, temp_buffer,
            (game->moved_down_last_tick) ? "true" : "false");
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  temp_buffer = "active_alien_bullets: %u";
  sprintf_s(buffer, 256, temp_buffer, game->alien_active_bullets);
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  for (u16 i = 0; i < MAX_PLAYER_BULLETS; ++i)
  {
    if (game->player_bullets[i].active)
    {
      sprintf_s(buffer, 256, "%u = {%f, %f}", i, game->player_bullets[i].pos.x,
                game->player_bullets[i].pos.y);
      DrawTextEx(game->debug_font, buffer,
                 (Vector2){(float)begin_x, (float)begin_y},
                 game->debug_font_size, 0, DARKGRAY);
      begin_y += 25;
    }
  }

  bool music_playing = IsMusicStreamPlaying(game->sound_assets.background);
  sprintf_s(buffer, 256, "%s",
            (music_playing) ? "music is playing" : "music is NOT playing");
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  Vector2 mouse_pos = GetMousePosition();
  temp_buffer = "mouse_pos = %.1f %.1f";
  sprintf_s(buffer, 256, temp_buffer, mouse_pos.x, mouse_pos.y);
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  Vector2 mouse_delta = GetMouseDelta();
  temp_buffer = "mouse_delta = %.1f %.1f";
  sprintf_s(buffer, 256, temp_buffer, mouse_delta.x, mouse_delta.y);
  DrawTextEx(game->debug_font, buffer,
             (Vector2){(float)begin_x, (float)begin_y}, game->debug_font_size,
             0, DARKGRAY);
  begin_y += 25;

  for (u16 i = 0; i < ALIENS_PER_COLUMN; ++i)
    DrawRectangleLinesEx(game->alien_rows[i], 1, GREEN);

  DrawRectangleLinesEx(game->debug_full_rectangle, 1.0, RED);
}

internal void Shoot(GameApp *game)
{
  if (game->player_active_bullets == MAX_PLAYER_BULLETS)
    return;

  f32 width = 4;
  f32 height = 14;
  f32 x = Centralize(game->player.pos.width, width) + game->player.pos.x;
  f32 y = game->player.pos.y - height;

  game->player_bullets[game->player_active_bullets].active = true;
  game->player_bullets[game->player_active_bullets].pos =
      (Rectangle){x, y, width, height};

  game->player_active_bullets++;

  PlaySound(game->sound_assets.laser);
}
