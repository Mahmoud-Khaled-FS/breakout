#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "raylib.h"

#define WIDTH 800
#define HEIGH 600
#define SPEED 0.1f

typedef struct
{
  Vector2 pos;
  Vector2 size;
} Entity;

Entity new_entity(float width, float height, float x, float y)
{
  Entity e = {
      .pos = {
          .x = width,
          .y = height,
      },
      .size = {
          .x = x,
          .y = y,
      }};
  return e;
}

//  x      x+w
//  10     110
//  [      ]
//     30[]

float collision(Entity *e1, Entity *e2)
{
  float e1_xs = e1->pos.x;
  float e1_xe = e1->pos.x + e1->size.x;
  float e2_xs = e2->pos.x;
  float e2_xe = e2->pos.x + e2->size.x;
  float e1_ys = e1->pos.y;
  float e1_ye = e1->pos.y + e1->size.y;
  float e2_ys = e2->pos.y;
  float e2_ye = e2->pos.y + e2->size.y;
  if (e1_xs <= e2_xe && e2_xs <= e1_xe && e1_ys <= e2_ye && e2_ys <= e1_ye)
  {
    float value = (e1_xe - e1->size.x / 2) - e2_xs;
    return -((value / 65) / 10);
  }
  return -1;
}

bool is_end(Entity *ball)
{
  if (ball->pos.y + ball->size.y >= HEIGH)
    return true;
  return false;
}

void move_ball(Entity *ball, Vector2 *dir)
{
  ball->pos.y += dir->y;
  ball->pos.x += dir->x;
}

char catch_edge(Entity *e)
{
  float e_xs = e->pos.x;
  float e_xe = e->pos.x + e->size.x;
  float e_ys = e->pos.y;
  float e_ye = e->pos.y + e->size.y;
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

int main()
{
  InitWindow(WIDTH, HEIGH, "breakout");
  float player_width = 100;
  float player_height = 20;
  Entity player = new_entity((WIDTH / 2) - (100 / 2), HEIGH - (HEIGH / 10), player_width, player_height);
  Entity ball = new_entity((WIDTH / 2) - 10, (HEIGH / 2) - 10, 20, 20);

  Vector2 ball_dir = {.x = 0.06, .y = SPEED};

  while (!WindowShouldClose())
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
      if (player.pos.x <= WIDTH - player_width)
      {
        player.pos.x += (1 * SPEED);
      }
    }

    float c = collision(&player, &ball);
    if (collision(&player, &ball) != -1)
    {
      ball_dir.y = -ball_dir.y;
      ball_dir.x = c;
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
      return 0;
    }
    move_ball(&ball, &ball_dir);

    BeginDrawing();
    ClearBackground(BLACK);
    DrawRectangleV(player.pos, player.size, YELLOW);
    DrawRectangleV(ball.pos, ball.size, WHITE);
    EndDrawing();
  }
  printf("hello world");
  return 0;
}
