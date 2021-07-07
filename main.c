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

Vector2 plus(Vector2 u, Vector2 v) {
  Vector2 w = {u.x + v.x, u.y + v.y};
  return w;
}

Vector2 minus(Vector2 u, Vector2 v) {
  Vector2 w = {u.x - v.x, u.y - v.y};
  return w;
}

Vector2 sTimes(float k, Vector2 v) {
  Vector2 w = {k * v.x, k * v.y};
  return w;
}

Vector2 sDivide(float k, Vector2 v) {
  Vector2 w = {v.x / k, v.y / k};
  return w;
}

Vector2 gridPositionConverter(float zoom, Vector2 gridCenter, Vector2 screenPostion, Vector2 screen) {
  Vector2 w = {0, 0};
  w = sDivide(zoom, plus(screenPostion, minus(gridCenter, sDivide(2, screen))));
  return w;
}

float squareDistance(Vector2 u, Vector2 v) {
  Vector2 w = minus(u, w);
  return powf(w.x, 2) - powf(w.y, 2);
}

int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  
  // screen
  const Vector2 screen = { 800, 450 };
  InitWindow(screen.x, screen.y, "Gravity Simulator");

  // mouse
  Vector2 wherePressed = { 0, 0 };
  Vector2 mousePosition = { -100.0f, -100.0f };
  unsigned int bodyFocus = 0, focus = 1;

  // grid
  Vector2 target = {0, 0};
  Camera2D camera = { 0 };
  camera.target = target;
  camera.offset = (Vector2){ screen.x / 2.0f, screen.y / 2.0f };
  camera.zoom = 1.0f;

  newBody(0, 0, 0, 0, 10000, 15);
  // newBody(0, 50, -4.47, 0, 1, 5);
  // newBody(0, -100, 3.16, 0, 1, 5);
  // newBody(150, 0, 0, 2.58, 1, 5);


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
    if (IsKeyPressed(KEY_SPACE)) pause = !pause;


    // mouse
    mousePosition = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
      target = mousePosition;
      wherePressed = camera.target;
    }
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
      Vector2 draged = sDivide(camera.zoom, minus(target, mousePosition));
      camera.target = plus(wherePressed, draged);
    }

    focus = 1;
    for (int body = 0; body < bodyNumber; body++) {
      Vector2 mousePositionInGrid = gridPositionConverter(camera.zoom, camera.target, mousePosition, screen);
      float distance = squareDistance(mousePositionInGrid, bodies[body].position);
      if (distance < powf(bodies[body].radius, 2)) {
        bodyFocus = body;
        focus = 0;
        break;
      }
    }

    float wheel = GetMouseWheelMove();
    if (wheel) {
      camera.zoom *= 0.5f * wheel + 1;
    }


    // simulation
    if (pause) {

      // forces
      for (int body = 0; body < bodyNumber; body++) {
        bodies[body].forces.x = 0;
        bodies[body].forces.y = 0;
      }
      for (int i = 0; i < bodyNumber - 1; i++) {
        for (int j = i + 1; j < bodyNumber; j++) {

          Vector2 distance2D = minus(bodies[i].position, bodies[j].position);
          float distance = powf(distance2D.x, 2) + powf(distance2D.y, 2);

          float bodyForce = gravityConstant * bodies[i].mass * bodies[j].mass / powf(distance, 1.5);

          bodies[i].forces = minus(bodies[i].forces, sTimes(bodyForce, distance2D));
          bodies[j].forces = plus(bodies[j].forces, sTimes(bodyForce, distance2D));
        }
      }

      // movimenty
      for (int body = 0; body < bodyNumber; body++) {
        bodies[body].velocity = plus(bodies[body].velocity, sDivide(bodies[body].mass, bodies[body].forces));
        bodies[body].position = plus(bodies[body].position, bodies[body].velocity);
      }
    }

    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
      BeginDrawing();

      ClearBackground((Color){20, 19, 22, 255});

      BeginMode2D(camera);
      for (int body = 0; body < bodyNumber; body++) {
        DrawCircle(
          bodies[body].position.x,
          bodies[body].position.y,
          bodies[body].radius, BLUE
        );
      }
      Vector2 debug3 = gridPositionConverter(camera.zoom, camera.target, mousePosition, screen);
      DrawCircle(debug3.x, debug3.y, 5, GREEN);
      EndMode2D();

      // só faz char str[16]; e snprintf(str, 16, "%d", num);

      DrawFPS(10, 10);
      if (!pause) DrawText("paused", 10, 30, 20, GRAY);
      char debug1[16];
      snprintf(debug1, 16, "%i", bodyFocus);
      DrawText(debug1, 10, 50, 20, GRAY);
      char debug2[16];
      snprintf(debug2, 16, "%i", focus);
      DrawText(debug2, 10, 70, 20, GRAY);
      

      EndDrawing();
      //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}