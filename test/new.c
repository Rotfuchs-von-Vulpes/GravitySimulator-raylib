#include "raylib.h"
#include <math.h>
#include <stdio.h>

// celestic bodys
struct celesticBody{
  Vector2 position;
  Vector2 velocity;
  Vector2 forces;
  float mass;
  float radius;
};

struct celesticBody bodies[50];

unsigned int bodyNumber = 0;

void newBody(float x, float y, float vx, float vy, float m, float r) {
  
  bodies[bodyNumber].position.x = x;
  bodies[bodyNumber].position.y = y;
  bodies[bodyNumber].velocity.x = vx;
  bodies[bodyNumber].velocity.y = vy;
  bodies[bodyNumber].mass = m;
  bodies[bodyNumber].radius = r;

  bodyNumber++;
}

int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  
  // screen
  const int screenWidth = 800;
  const int screenHeight = 450;
  InitWindow(screenWidth, screenHeight, "Gravity Simulator");

  // mouse
  // Vector2 mousePosition = { -100.0f, -100.0f };

  // grid
  float dx = 0;
  float dy = 0;

  newBody(400, 200, 0, 0, 10000, 15);
  newBody(400, 100, 3.16, 0, 1, 5);
  newBody(400, 250, -4.47, 0, 1, 5);
  newBody(550, 0, 0, 2.58, 1, 5);


  // gravity simulation
  short int pause = 1;
  const float_t gravityConstant = 0.1;

  SetTargetFPS(60);
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) {
    // Update
    //----------------------------------------------------------------------------------
    
    // keyboard
    if (IsKeyDown(KEY_RIGHT)) dx += 2.0f;
    if (IsKeyDown(KEY_LEFT)) dx -= 2.0f;
    if (IsKeyDown(KEY_UP)) dy -= 2.0f;
    if (IsKeyDown(KEY_DOWN)) dy += 2.0f;
    if (IsKeyPressed(KEY_SPACE)) pause = !pause;

    // mouse
    // mousePosition = GetMousePosition();

    if (pause) {
      // forces

      for (int body = 0; body < bodyNumber; body++) {
        bodies[body].forces.x = 0;
        bodies[body].forces.y = 0;
      }

      for (int i = 0; i < bodyNumber - 1; i++) {
        for (int j = i + 1; j < bodyNumber; j++) {

          Vector2 distance2D;
          distance2D.x = bodies[i].position.x - bodies[j].position.x;
          distance2D.y = bodies[i].position.y - bodies[j].position.y;
          float distance = powf(distance2D.x, 2) + powf(distance2D.y, 2);

          float bodyForce = gravityConstant * bodies[i].mass * bodies[j].mass / powf(distance, 1.5);
          bodies[i].forces.x -= distance2D.x * bodyForce;
          bodies[i].forces.y -= distance2D.y * bodyForce;
          bodies[j].forces.x += distance2D.x * bodyForce;
          bodies[j].forces.y += distance2D.y * bodyForce;
        }
      }

      // movimenty
      for (int body = 0; body < bodyNumber; body++) {
        bodies[body].velocity.x += bodies[body].forces.x / bodies[body].mass;
        bodies[body].velocity.y += bodies[body].forces.y / bodies[body].mass;
        bodies[body].position.x += bodies[body].velocity.x;
        bodies[body].position.y += bodies[body].velocity.y;
      }
    }

    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
      BeginDrawing();

      ClearBackground(RAYWHITE);

      for (int body = 0; body < bodyNumber; body++) {
        DrawCircle(
          bodies[body].position.x - dx,
          bodies[body].position.y - dy,
          bodies[body].radius, BLUE
        );
        
        DrawFPS(10, 10);
        if (!pause) DrawText("paused", 10, 30, 20, DARKGRAY);}

      EndDrawing();
      //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}