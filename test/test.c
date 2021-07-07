// #include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

struct celesticBody {
  float position[2];
  float velocity[2];
  float forces[2];
  float mass;
  float radius;
};

// celestic bodys
// float bodyPosition[50][2];
// float bodyVelocity[50][2];
// float bodyForces[50][2];
// float bodyMass[50];
// float bodyRadius[50];
unsigned int bodyNumber = 0;
struct celesticBody bodies[50];

void newBody(float x, float y, float vx, float vy, float m, float r) {
  bodies[bodyNumber].position[0] = x;
  bodies[bodyNumber].position[1] = y;
  bodies[bodyNumber].velocity[0] = vx;
  bodies[bodyNumber].velocity[1] = vy;
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
  // InitWindow(screenWidth, screenHeight, "Gravity Simulator");

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

  // SetTargetFPS(60);
  //--------------------------------------------------------------------------------------

  // Main game loop
  int iterator = 0;
  while (iterator < 10) {
    // Update
    //----------------------------------------------------------------------------------
    
    // keyboard
    // if (IsKeyDown(KEY_RIGHT)) dx += 2.0f;
    // if (IsKeyDown(KEY_LEFT)) dx -= 2.0f;
    // if (IsKeyDown(KEY_UP)) dy -= 2.0f;
    // if (IsKeyDown(KEY_DOWN)) dy += 2.0f;

    // mouse
    // mousePosition = GetMousePosition();

    // forces

    for (int body = 0; body < bodyNumber; body++) {
      bodies[body].forces[0] = 0;
      bodies[body].forces[1] = 0;
    }

    for (int i = 0; i < bodyNumber - 1; i++) {
      for (int j = i + 1; j < bodyNumber; j++) {

        printf("setando gravidade entre %i e %i \n\n", i, j);
        printf("corpos nas posicões (%f, %f) e (%f, %f), nas velocidades [%f, %f] e [%f, %f], com forças [%f, %f] e [%f, %f] \n", 
        bodies[i].position[0], bodies[i].position[1], 
        bodies[j].position[0], bodies[j].position[1], 
        bodies[i].velocity[0], bodies[i].velocity[1], 
        bodies[j].velocity[0], bodies[j].velocity[1], 
        bodies[i].forces[0], bodies[i].forces[1],
        bodies[j].forces[0], bodies[j].forces[1]);

        float distance2D[2];
        distance2D[0] = bodies[i].position[0] - bodies[j].position[0];
        distance2D[1] = bodies[i].position[1] - bodies[j].position[1];
        float distance = powf(distance2D[0], 2) + powf(distance2D[1], 2);

        float bodyForce = gravityConstant * bodies[i].mass * bodies[j].mass / powf(distance, 1.5);
        bodies[i].forces[0] -= distance2D[0] * bodyForce;
        bodies[i].forces[1] -= distance2D[1] * bodyForce;
        bodies[j].forces[0] += distance2D[0] * bodyForce;
        bodies[j].forces[1] += distance2D[1] * bodyForce;

        printf("corpos nas posicões (%f, %f) e (%f, %f), nas velocidades [%f, %f] e [%f, %f], com forças [%f, %f] e [%f, %f] \n", 
        bodies[i].position[0], bodies[i].position[1], 
        bodies[j].position[0], bodies[j].position[1], 
        bodies[i].velocity[0], bodies[i].velocity[1], 
        bodies[j].velocity[0], bodies[j].velocity[1], 
        bodies[i].forces[0], bodies[i].forces[1],
        bodies[j].forces[0], bodies[j].forces[1]);
      }
    }

    // movimenty
    for (int body = 0; body < bodyNumber; body++) {
      bodies[body].velocity[0] += bodies[body].forces[0] / bodies[body].mass;
      bodies[body].velocity[1] += bodies[body].forces[1] / bodies[body].mass;
      bodies[body].velocity[0] += bodies[body].velocity[0];
      bodies[body].velocity[1] += bodies[body].velocity[1];
    }
    iterator++;
  }

  system("pause");
  return 0;
}