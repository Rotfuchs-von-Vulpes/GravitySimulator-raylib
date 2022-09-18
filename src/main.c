#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define gravityConstant 0.005
#define density 1

#include "utils/bodies.c"
#include "utils/vector.c"
#include "utils/simulation.c"

#include "views/icons.c"
#include "views/inputs.c"
#include "views/draw.c"

int main(void)
{
	InitWindow(screen.x, screen.y, "Gravity Simulator");

	bodies = (CelesticBody *)malloc(5 * sizeof(CelesticBody));

	SetTargetFPS(60);

	Camera2D camera = {0};
	camera.target = target;
	camera.offset = (Vector2){screen.x / 2.0f, screen.y / 2.0f};
	camera.zoom = 1.0f;

	while (!WindowShouldClose())
	{
		camera = inputs(camera);

		// simulation
		if (!pause && bodyCount > 0)
		{
			simulation();
		}

		// grid move
		if (focus)
		{
			camera.target = bodies[bodyTarget].position;
		}

		draw(camera);
	}
	
	CloseWindow(); // Close window and OpenGL context

	return 0;
}
