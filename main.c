#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "raylib.h"

#define WIDTH 1200
#define HEIGH 800
#define SPEED 10.0f
#define PLAYER_WIDTH 200.0f
#define PLAYER_HEIGHT 20.0f
#define BALL_RADIUS 10.0f
#define BLOCK_WIDTH 80
#define MAX_LINE_BLOCK_COUNT 12
#define BLOCK_COUNT 6 * MAX_LINE_BLOCK_COUNT
#define PADDING (WIDTH - (BLOCK_WIDTH * MAX_LINE_BLOCK_COUNT)) / (MAX_LINE_BLOCK_COUNT + 1)

typedef struct
{
  Vector2 pos;
  Vector2 size;
  bool visible;
  Color color;
} Entity;

typedef struct
{
  Vector2 pos;
  float radius;
  Color color;
} Ball;

Entity new_entity(float x, float y, float width, float height, Color color)
{
  Entity e = {
      .pos = {
          .x = x,
          .y = y,
      },
      .size = {
          .x = width,
          .y = height,
      },
      .visible = true,
      .color = color,
  };
  return e;
}

float collision(Entity *entity, Ball *ball)
{
  float entity_xs = entity->pos.x;
  float entity_xe = entity->pos.x + entity->size.x;
  float ball_xs = ball->pos.x - ball->radius;
  float ball_xe = ball->pos.x + ball->radius;

  float entity_ys = entity->pos.y;
  float entity_ye = entity->pos.y + entity->size.y;
  float ball_ys = ball->pos.y - ball->radius;
  float ball_ye = ball->pos.y + ball->radius;

  if (entity_xs <= ball_xe && ball_xs <= entity_xe && entity_ys <= ball_ye && ball_ys <= entity_ye && entity->visible)
  {
    float center_entity = entity_xe - entity->size.x / 2;
    float center_ball = ball->pos.x;
    float dist = center_entity - center_ball;
    return -(dist / (entity->size.x / 2) * 5);
  }
  return -999;
}

void move_ball(Ball *ball, Vector2 *dir)
{
  ball->pos.y += dir->y;
  ball->pos.x += dir->x;
}

char catch_edge(Ball *e)
{
  float e_xs = e->pos.x - e->radius;
  float e_xe = e->pos.x + e->radius;
  float e_ys = e->pos.y - e->radius;
  float e_ye = e->pos.y + e->radius;
  if (e_xs <= 0)
    return 'l';
  if (e_xe >= WIDTH)
    return 'r';
  if (e_ys <= 0)
    return 't';
  if (e_ye >= HEIGH)
    return 'b';
  return 0;
}

void create_blocks(Entity *list, int size)
{
  for (int i = 0; i < size; i++)
  {
    int line = i / MAX_LINE_BLOCK_COUNT;
    int row = i - line * MAX_LINE_BLOCK_COUNT;
    float hue = (float)i / MAX_LINE_BLOCK_COUNT;
    Color color = ColorFromHSV(hue * 360, 1.0f, 1.0f);
    Entity b = new_entity(PADDING + (row * (BLOCK_WIDTH + PADDING)), 40 + (line * (20 + 10)), BLOCK_WIDTH, 20, color);
    list[i] = b;
    DrawRectangleV(b.pos, b.size, YELLOW);
  }
  return;
}
void render_blocks(Entity *list, int size)
{
  for (int i = 0; i < size; i++)
  {
    Entity b = list[i];
    if (b.visible)
    {
      DrawRectangleV(b.pos, b.size, b.color);
    }
  }
  return;
}

Entity create_player()
{
  return new_entity((WIDTH / 2) - (PLAYER_WIDTH / 2), HEIGH - (HEIGH / 10), PLAYER_WIDTH, PLAYER_HEIGHT, YELLOW);
}

Ball create_ball()
{
  return (Ball){
      .radius = BALL_RADIUS,
      .pos = {
          .x = WIDTH / 2,
          .y = HEIGH / 2},
      .color = WHITE,
  };
  // return new_entity((WIDTH / 2) - 10, (HEIGH / 2) - 10, 13, 13, WHITE);
}
Vector2 reset_ball()
{
  return (Vector2){
      .x = 0,
      .y = SPEED,
  };
}

void render_text_in_center(char *text, int fs, int y, Color color)
{
  if (y == -1)
  {
    y = HEIGH / 2 - 35 / 2;
  }
  int size = MeasureText(text, fs);
  DrawText(text, (WIDTH / 2) - size / 2, y, fs, color);
}
int main()
{
  InitWindow(WIDTH, HEIGH, "breakout");
  SetTargetFPS(60);
  InitAudioDevice();
  Sound bamboo = LoadSound("./Bamboo.mp3");
  // State
  Entity player = create_player();
  Ball ball = create_ball();
  Vector2 ball_dir = reset_ball();
  Entity blocks[BLOCK_COUNT] = {0};
  create_blocks(blocks, BLOCK_COUNT);
  int score = 0;
  char *score_text = malloc(10);

  bool game_loop = false;
  bool is_finished = false;
  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);
    if (IsKeyPressed(KEY_R))
    {
      if (is_finished)
      {
        player = create_player();
        ball = create_ball();
        ball_dir = reset_ball();
        create_blocks(blocks, BLOCK_COUNT);
        score = 0;
        is_finished = false;
        game_loop = true;
      }
    }
    if (IsKeyPressed(KEY_Q))
    {
      UnloadSound(bamboo);
      CloseWindow();
      return 0;
    }
    if (IsKeyPressed(KEY_SPACE))
    {
      game_loop = !game_loop;
    }
    if (!game_loop)
    {
      char *title = "";
      if (is_finished)
      {
        title = "GAME OVER";
        sprintf(score_text, "Score: %d", score);
        render_text_in_center(score_text, 35, HEIGH / 2 + 60, RED);
        render_text_in_center("Press r to restart.", 27, HEIGH / 2 + 120, WHITE);
        render_text_in_center("Press q to exit.", 27, HEIGH / 2 + 160, WHITE);
      }
      else
      {
        title = "Press Space To Start the game";
        render_text_in_center("Press q to exit.", 27, HEIGH / 2 + 60, WHITE);
      }
      render_text_in_center(title, 45, -1, WHITE);
    }
    else
    {

      if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
      {
        if (player.pos.x >= 0)
        {
          player.pos.x -= (1 * SPEED);
        }
      }
      if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
      {
        if (player.pos.x <= WIDTH - PLAYER_WIDTH)
        {
          player.pos.x += (1 * SPEED);
        }
      }

      float c = collision(&player, &ball);
      if (c != -999)
      {
        ball_dir.y = -ball_dir.y;
        ball_dir.x = c;
        PlaySound(bamboo);
      }

      for (int i = 0; i < BLOCK_COUNT; i++)
      {
        float c = collision(&blocks[i], &ball);
        if (c != -999)
        {
          ball_dir.y = -ball_dir.y;
          ball_dir.x = c;
          blocks[i].visible = false;
          PlaySound(bamboo);
          score += 1;
          if (score == BLOCK_COUNT)
          {
            game_loop = false;
            is_finished = true;
          }
        }
      }

      char edge = catch_edge(&ball);
      if (edge == 'r' || edge == 'l')
      {
        ball_dir.x = -ball_dir.x;
      }
      else if (edge == 't')
      {
        ball_dir.y = -ball_dir.y;
      }
      else if (edge == 'b')
      {
        game_loop = false;
        is_finished = true;
      }
      move_ball(&ball, &ball_dir);

      sprintf(score_text, "Score: %d", score);
      DrawText(score_text, 12, 10, 16, WHITE);
      // DrawRectangleV(ball.pos, ball.size, ball.color);
    }
    DrawCircleV(ball.pos, ball.radius, ball.color);
    DrawRectangleV(player.pos, player.size, player.color);
    render_blocks(blocks, BLOCK_COUNT);
    EndDrawing();
  }
  return 0;
}