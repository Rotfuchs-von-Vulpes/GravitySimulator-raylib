#include "raylib.h"
#include <math.h>
#include <stdio.h>

// celestic bodys
struct celesticBody
{
	Vector2 position;
	Vector2 velocity;
	Vector2 forces;
	float mass;
	float radius;
};

struct celesticBody bodies[50];
unsigned int bodyNumber = 0;

enum modes
{
	Drag,
	Create,
	Shoot,
	Remove
} mode = Drag;

void newBody(float x, float y, float vx, float vy, float m, float r)
{

	bodies[bodyNumber].position.x = x;
	bodies[bodyNumber].position.y = y;
	bodies[bodyNumber].velocity.x = vx;
	bodies[bodyNumber].velocity.y = vy;
	bodies[bodyNumber].mass = m;
	bodies[bodyNumber].radius = r;

	bodyNumber++;
}

void removeBody(unsigned int index)
{
	if (index > bodyNumber)
	{
		return;
	}
	else if (index == bodyNumber)
	{
		bodyNumber--;
		return;
	}

	for (size_t i = index; i < bodyNumber - 1; i++)
	{
		bodies[i] = bodies[i + 1];
	}

	bodyNumber--;
}

Vector2 plus(Vector2 u, Vector2 v)
{
	return (Vector2){u.x + v.x, u.y + v.y};
}

Vector2 minus(Vector2 u, Vector2 v)
{
	return (Vector2){u.x - v.x, u.y - v.y};
}

Vector2 sTimes(float k, Vector2 v)
{
	return (Vector2){k * v.x, k * v.y};
}

Vector2 sDivide(float k, Vector2 v)
{
	return (Vector2){v.x / k, v.y / k};
}

Vector2 gridPositionConverter(float zoom, Vector2 gridCenter, Vector2 screenPostion, Vector2 screen)
{
	return (Vector2)(plus(sDivide(zoom, minus(screenPostion, sDivide(2, screen))), gridCenter));
}

float squareDistance(Vector2 u, Vector2 v)
{
	Vector2 w = minus(u, w);
	return (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
}

float collision(Vector2 positionA, Vector2 positionB, float distance)
{
	return (bool)(squareDistance(positionA, positionB) < distance);
}

void drawDragIcon(Vector2 position, Color color)
{
	DrawCircle(position.x, position.y, 20, color);
}

void drawCreateIcon(Vector2 position, Color color)
{
	DrawRectangle(position.x - 5, position.y - 20, 10, 40, color);
	DrawRectangle(position.x - 20, position.y - 5, 40, 10, color);
}

void drawRemoveIcon(Vector2 position, Color color)
{
	DrawRectanglePro((Rectangle){position.x, position.y, 10, 40}, (Vector2){5, 20}, 135, color);
	DrawRectanglePro((Rectangle){position.x, position.y, 10, 40}, (Vector2){5, 20}, 45, color);
}

int main(void)
{
	// Initialization
	//--------------------------------------------------------------------------------------

	// screen
	const Vector2 screen = {800, 450};
	InitWindow(screen.x, screen.y, "Gravity Simulator");

	// mouse
	Vector2 wherePressed = {0, 0};
	bool canDrag = true;
	Vector2 mousePosition = {-100.0f, -100.0f};
	size_t bodyTarget = 0, bodyFocus = 0;
	bool objectInCursor = false;

	// grid
	Vector2 target = {0, 0};
	Camera2D camera = {0};
	camera.target = target;
	camera.offset = (Vector2){screen.x / 2.0f, screen.y / 2.0f};
	camera.zoom = 1.0f;
	bool focus = false;

	// bodies
	newBody(0, 0, 0, 0, 10000, 15);
	newBody(0, 50, -4.47, 0, 1, 5);
	newBody(0, -100, 3.16, 0, 1, 5);
	newBody(150, 0, 0, 2.58, 1, 5);

	// gravity simulation
	bool pause = false;
	short unsigned int simulationSpeed = 1;
	const float_t gravityConstant = 0.1;

	// debug
	bool debug = false;

	SetTargetFPS(60);
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose())
	{
		// Update
		//----------------------------------------------------------------------------------

		// keyboard
		if (IsKeyPressed(KEY_SPACE))
			pause = !pause;
		if (IsKeyPressed(KEY_F1))
			debug = !debug;
		if (IsKeyPressed(KEY_COMMA) && simulationSpeed > 1)
			simulationSpeed--;
		if (IsKeyPressed(KEY_PERIOD) && simulationSpeed < 5)
			simulationSpeed++;

		// mouse
		mousePosition = GetMousePosition();
		Vector2 mousePositionInGrid = gridPositionConverter(camera.zoom, camera.target, mousePosition, screen);

		if (mousePosition.y <= screen.y - 50)
		{
			// move camera
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
			{
				target = mousePosition;
				wherePressed = camera.target;
				canDrag = true;
				focus = false;
			}
			if (canDrag && IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
			{
				Vector2 draged = sDivide(camera.zoom, minus(target, mousePosition));
				camera.target = plus(wherePressed, draged);
			}
			if (IsMouseButtonUp(MOUSE_RIGHT_BUTTON))
			{
				canDrag = false;
			}

			if (mode == Drag || mode == Remove)
			{
				// find body in a cursor
				objectInCursor = 0;
				for (size_t body = 0; body < bodyNumber; body++)
				{
					if (collision(bodies[body].position, mousePositionInGrid, bodies[body].radius * bodies[body].radius))
					{
						bodyFocus = body;
						objectInCursor = true;
						break;
					}
				}
			}

			switch (mode)
			{
			case Drag:
				// focus in a body
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					if (objectInCursor)
					{
						focus = true;
						bodyTarget = bodyFocus;
					}
					else
					{
						focus = false;
					}
				}

				break;
			case Create:

				break;
			case Shoot:

				break;
			case Remove:
				// remove a body
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					if (objectInCursor)
					{
						removeBody(bodyFocus);
					}
				}

				break;
			}

			float wheel = GetMouseWheelMove();
			if (wheel)
			{
				if (mode == Create)
				{
				}
				else
				{
					camera.zoom *= 0.5f * wheel + 1;
					if (!focus)
					{
						camera.target = mousePositionInGrid;
						SetMousePosition(screen.x / 2, screen.y / 2);
					}
				}
			}
		}
		else
		{
			// buttons
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				if (mousePosition.x >= 0 && mousePosition.x < screen.x / 3)
				{
					mode = Drag;
				}
				else if (mousePosition.x >= screen.x / 3 && mousePosition.x < 2 * screen.x / 3)
				{
					mode = Create;
				}
				else if (mousePosition.x >= 2 * screen.x / 3 && mousePosition.x < screen.x)
				{
					mode = Remove;
				}

				// unfocus
				if (focus && mode != Drag)
					focus = false;
			}
		}

		// simulation
		if (!pause)
		{
			for (size_t time = 0; time < simulationSpeed; time++)
			{
				// forces
				for (size_t body = 0; body < bodyNumber; body++)
				{
					bodies[body].forces.x = 0;
					bodies[body].forces.y = 0;
				}
				for (size_t i = 0; i < bodyNumber - 1; i++)
				{
					for (size_t j = i + 1; j < bodyNumber; j++)
					{

						Vector2 distance2D = minus(bodies[i].position, bodies[j].position);
						float distance = powf(distance2D.x, 2) + powf(distance2D.y, 2);

						float bodyForce = gravityConstant * bodies[i].mass * bodies[j].mass / powf(distance, 1.5);

						bodies[i].forces = minus(bodies[i].forces, sTimes(bodyForce, distance2D));
						bodies[j].forces = plus(bodies[j].forces, sTimes(bodyForce, distance2D));
					}
				}

				// movimenty
				for (size_t body = 0; body < bodyNumber; body++)
				{
					bodies[body].velocity = plus(bodies[body].velocity, sDivide(bodies[body].mass, bodies[body].forces));
					bodies[body].position = plus(bodies[body].position, bodies[body].velocity);
				}
			}
		}

		// grid move
		if (focus)
		{
			camera.target = bodies[bodyTarget].position;
		}

		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground((Color){20, 19, 22, 255});

		BeginMode2D(camera);

		Color color;

		if (mode == Drag)
		{
			color = WHITE;
		}
		else if (mode == Remove)
		{
			color = RED;
		}

		if (objectInCursor)
		{
			DrawCircle(bodies[bodyFocus].position.x, bodies[bodyFocus].position.y, bodies[bodyFocus].radius + 2, color);
		}

		for (size_t body = 0; body < bodyNumber; body++)
		{
			DrawCircle(
					bodies[body].position.x,
					bodies[body].position.y,
					bodies[body].radius, BLUE);
		}

		EndMode2D();

		DrawRectangle(0, screen.y - 50, screen.x, 50, GRAY);

		Color dragIconColor = WHITE;
		Color createIconColor = WHITE;
		Color removeIconColor = WHITE;

		switch (mode)
		{
		case Drag:
			dragIconColor = ORANGE;
			break;
		case Create:
		case Shoot:
			createIconColor = ORANGE;
			break;
		case Remove:
			removeIconColor = ORANGE;
			break;
		}

		drawDragIcon((Vector2){screen.x / 4, screen.y - 25}, dragIconColor);
		drawCreateIcon((Vector2){2 * screen.x / 4, screen.y - 25}, createIconColor);
		drawRemoveIcon((Vector2){3 * screen.x / 4, screen.y - 25}, removeIconColor);

		DrawFPS(10, 10);
		if (!pause)
			DrawText("paused", 10, 30, 20, GRAY);
		char speedText[16];
		snprintf(speedText, 16, "%ix", simulationSpeed);
		DrawText(speedText, 10, 50, 20, GRAY);

		if (debug)
		{
			Vector2 debug3 = gridPositionConverter(camera.zoom, camera.target, mousePosition, screen);
			DrawCircle(debug3.x, debug3.y, 5, GREEN);

			char debug1[16];
			snprintf(debug1, 16, "%i", bodyTarget);
			DrawText(debug1, 10, 70, 20, GRAY);

			char debug2[16];
			snprintf(debug2, 16, "%i", objectInCursor);
			DrawText(debug2, 10, 90, 20, GRAY);
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
