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
  return (Vector2){ u.x + v.x, u.y + v.y };
}

Vector2 minus(Vector2 u, Vector2 v) {
  return (Vector2){ u.x - v.x, u.y - v.y };
}

Vector2 sTimes(float k, Vector2 v) {
  return (Vector2){ k * v.x, k * v.y };
}

Vector2 sDivide(float k, Vector2 v) {
  return (Vector2){ v.x / k, v.y / k };
}

Vector2 gridPositionConverter(float zoom, Vector2 gridCenter, Vector2 screenPostion, Vector2 screen) {
  return (Vector2)(plus(sDivide(zoom, minus(screenPostion, sDivide(2, screen))), gridCenter));
}

float squareDistance(Vector2 u, Vector2 v) {
  Vector2 w = minus(u, w);
  return powf((u.x - v.x), 2) + powf((u.y - v.y), 2);
}

float collision(Vector2 positionA, Vector2 positionB, float distance) {
  return (bool)(squareDistance(positionA, positionB) < distance);
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
  size_t bodyTarget = 0, bodyFocus = 0;
  bool ObjectInCursor = 0;

  // grid
  Vector2 target = { 0, 0 };
  Camera2D camera = { 0 };
  camera.target = target;
  camera.offset = (Vector2){ screen.x / 2.0f, screen.y / 2.0f };
  camera.zoom = 1.0f;
  bool focus = 0;

  // bodies
  newBody(0, 0, 0, 0, 10000, 15);
  newBody(0, 50, -4.47, 0, 1, 5);
  newBody(0, -100, 3.16, 0, 1, 5);
  newBody(150, 0, 0, 2.58, 1, 5);

  // gravity simulation
  bool pause = 1;
  const float_t gravityConstant = 0.1;

  // debug
  bool debug = 0;

  SetTargetFPS(60);
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) {
    // Update
    //----------------------------------------------------------------------------------
    
    // keyboard
    if (IsKeyPressed(KEY_SPACE)) pause = !pause;
    if (IsKeyPressed(KEY_F1)) debug = !debug;

    // mouse
    mousePosition = GetMousePosition();
    Vector2 mousePositionInGrid = gridPositionConverter(camera.zoom, camera.target, mousePosition, screen);

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
      target = mousePosition;
      wherePressed = camera.target;
      focus = 0;
    }
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
      Vector2 draged = sDivide(camera.zoom, minus(target, mousePosition));
      camera.target = plus(wherePressed, draged);
    }

    ObjectInCursor = 0;
    for (size_t body = 0; body < bodyNumber; body++) {
      if (collision(bodies[body].position, mousePositionInGrid, powf(bodies[body].radius, 2))) {
        bodyFocus = body;
        ObjectInCursor = 1;
        break;
      }
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (ObjectInCursor) {
        focus = 1;
        bodyTarget = bodyFocus;
      } else {
        focus = 0;
      }
    }

    float wheel = GetMouseWheelMove();
    if (wheel) {
      camera.zoom *= 0.5f * wheel + 1;
    }

    // simulation
    if (pause) {

      // forces
      for (size_t body = 0; body < bodyNumber; body++) {
        bodies[body].forces.x = 0;
        bodies[body].forces.y = 0;
      }
      for (size_t i = 0; i < bodyNumber - 1; i++) {
        for (size_t j = i + 1; j < bodyNumber; j++) {

          Vector2 distance2D = minus(bodies[i].position, bodies[j].position);
          float distance = powf(distance2D.x, 2) + powf(distance2D.y, 2);

          float bodyForce = gravityConstant * bodies[i].mass * bodies[j].mass / powf(distance, 1.5);

          bodies[i].forces = minus(bodies[i].forces, sTimes(bodyForce, distance2D));
          bodies[j].forces = plus(bodies[j].forces, sTimes(bodyForce, distance2D));
        }
      }

      // movimenty
      for (size_t body = 0; body < bodyNumber; body++) {
        bodies[body].velocity = plus(bodies[body].velocity, sDivide(bodies[body].mass, bodies[body].forces));
        bodies[body].position = plus(bodies[body].position, bodies[body].velocity);
      }
    }

    // grid
    if (focus) {
      camera.target = bodies[bodyTarget].position;
    }

    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
      BeginDrawing();

      ClearBackground((Color){20, 19, 22, 255});

      BeginMode2D(camera);

      if (ObjectInCursor) {
        DrawCircle(bodies[bodyFocus].position.x, bodies[bodyFocus].position.y, bodies[bodyFocus].radius + 2, WHITE);
      }

      for (size_t body = 0; body < bodyNumber; body++) {
        DrawCircle(
          bodies[body].position.x,
          bodies[body].position.y,
          bodies[body].radius, BLUE
        );
      }

      if (debug) {
        Vector2 debug3 = gridPositionConverter(camera.zoom, camera.target, mousePosition, screen);
        DrawCircle(debug3.x, debug3.y, 5, GREEN);
        for (size_t body = 0; body < bodyNumber; body++) {
          Vector2 bodyPosition = bodies[body].position;
          DrawLine(bodyPosition.x, bodyPosition.y, debug3.x, debug3.y, RED);
          float distance = powf(squareDistance(bodyPosition, debug3), 0.5f);
          Vector2 textPosition = sDivide(2, plus(bodyPosition, debug3));
          char debug1[16];
          snprintf(debug1, 16, "%f", distance);
          DrawText(debug1, textPosition.x, textPosition.y, 5, RED);
        }
      }

      EndMode2D();

      // só faz char str[16]; e snprintf(str, 16, "%d", num);

      DrawFPS(10, 10);
      if (!pause) DrawText("paused", 10, 30, 20, GRAY);

      if (debug) {
        char debug1[16];
        snprintf(debug1, 16, "%i", bodyTarget);
        DrawText(debug1, 10, 50, 20, GRAY);
        char debug2[16];
        snprintf(debug2, 16, "%i", ObjectInCursor);
        DrawText(debug2, 10, 70, 20, GRAY);
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