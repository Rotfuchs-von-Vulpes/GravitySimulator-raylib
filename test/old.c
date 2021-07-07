#include "raylib.h"
#include <math.h>

// celestic bodys
float bodyPosition[50][2];
float bodyVelocity[50][2];
float bodyForces[50][2];
float bodyMass[50];
float bodyRadius[50];
unsigned int bodyNumber = 0;

void newBody(float x, float y, float vx, float vy, float m, float r) {
  bodyPosition[bodyNumber][0] = x;
  bodyPosition[bodyNumber][1] = y;
  bodyVelocity[bodyNumber][0] = vx;
  bodyVelocity[bodyNumber][1] = vy;
  bodyMass[bodyNumber] = m;
  bodyRadius[bodyNumber] = r;
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


  // gravity
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

    // mouse
    // mousePosition = GetMousePosition();

    // forces
    float bodyForces[bodyNumber][2];

    for (int body = 0; body < bodyNumber; body++) {
      bodyForces[body][0] = 0;
      bodyForces[body][1] = 0;
    }

    for (int i = 0; i < bodyNumber - 1; i++) {
      for (int j = i + 1; j < bodyNumber; j++) {
        float distance2D[2];
        distance2D[0] = bodyPosition[i][0] - bodyPosition[j][0];
        distance2D[1] = bodyPosition[i][1] - bodyPosition[j][1];
        float distance = powf(distance2D[0], 2) + powf(distance2D[1], 2);

        float bodyForce = gravityConstant * bodyMass[i] * bodyMass[j] / powf(distance, 1.5);
        bodyForces[i][0] -= distance2D[0] * bodyForce;
        bodyForces[i][1] -= distance2D[1] * bodyForce;
        bodyForces[j][0] += distance2D[0] * bodyForce;
        bodyForces[j][1] += distance2D[1] * bodyForce;
      }
    }

    // movimenty
    for (int body = 0; body < bodyNumber; body++) {
      bodyVelocity[body][0] += bodyForces[body][0] / bodyMass[body];
      bodyVelocity[body][1] += bodyForces[body][1] / bodyMass[body];
      bodyPosition[body][0] += bodyVelocity[body][0];
      bodyPosition[body][1] += bodyVelocity[body][1];
    }

    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
      BeginDrawing();

      ClearBackground(RAYWHITE);

      for (int body = 0; body < bodyNumber; body++) {
        DrawCircle(bodyPosition[body][0] + dx, bodyPosition[body][1] + dy, bodyRadius[body], BLUE);

        // vectors
        float forceX = bodyForces[body][0] + bodyPosition[body][0];
        float forceY = bodyForces[body][1] + bodyPosition[body][1];
        float velocityX = bodyVelocity[body][0] + bodyPosition[body][0];
        float velocityY = bodyVelocity[body][1] + bodyPosition[body][1];
        DrawLine(bodyPosition[body][0], bodyPosition[body][1], forceX, forceY, RED);
        DrawLine(bodyPosition[body][0], bodyPosition[body][1], velocityX, velocityY, RED);
      }

      EndDrawing();
      //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}