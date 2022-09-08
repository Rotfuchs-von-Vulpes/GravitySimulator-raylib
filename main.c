#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define gravityConstant 0.005
#define density 1

// celestic bodys
struct CelesticBody
{
	Vector2 position;
	Vector2 velocity;
	Vector2 forces;
	float mass;
	float radius;
};

struct CelesticBody bodies[50];
unsigned int bodyNumber = 0;

char debugText1[16] = "";
char debugText2[16] = "";

void newBody(float x, float y, float vx, float vy, float m)
{
	bodies[bodyNumber].position.x = x;
	bodies[bodyNumber].position.y = y;
	bodies[bodyNumber].velocity.x = vx;
	bodies[bodyNumber].velocity.y = vy;
	bodies[bodyNumber].mass = m;
	bodies[bodyNumber].radius = cbrtf(4 * m / (3 * density * PI));

	bodyNumber++;
}

void removeBody(unsigned int index)
{
	if (index == bodyNumber)
	{
		bodyNumber--;
	}
	else if (index < bodyNumber)
	{
		bodies[index] = bodies[--bodyNumber];
	}
}

void orbitBody(unsigned int index, float x, float y, float m)
{
	struct CelesticBody body = bodies[index];

	float m2 = body.mass;
	Vector2 v2 = body.velocity;
	Vector2 p = body.position;
	Vector2 q = {x, y};

	float dx = p.x - q.x;
	float dy = p.y - q.y;
	float distance = sqrtf(dx * dx + dy * dy);
	float realVelocity = sqrtf(m2 * gravityConstant / distance);
	float bodyVelocity = sqrtf(m2 * gravityConstant / (distance * distance * distance));

	float vx = bodyVelocity * dy;
	float vy = bodyVelocity * dx;

	snprintf(debugText1, 16, "%f", realVelocity);
	snprintf(debugText1, 16, "%f", sqrtf(vx*vx + vy*vy));

	newBody(q.x, q.y, -vx + v2.x, vy + v2.y, m);
}

void bodyCollision(unsigned int index1, unsigned int index2)
{
	struct CelesticBody body1 = bodies[index1];
	struct CelesticBody body2 = bodies[index2];

	removeBody(index2);
	removeBody(index1);

	float x = (body1.mass * body1.position.x + body2.mass * body2.position.x) / (body1.mass + body2.mass);
	float y = (body1.mass * body1.position.y + body2.mass * body2.position.y) / (body1.mass + body2.mass);
	float vx = (body1.mass * body1.velocity.x + body2.mass * body2.velocity.x) / (body1.mass + body2.mass);
	float vy = (body1.mass * body1.velocity.y + body2.mass * body2.velocity.y) / (body1.mass + body2.mass);

	float m = body1.mass + body2.mass;

	newBody(x, y, vx, vy, m);
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

	// modes rules
	enum modes
	{
		Drag,
		Create,
		Shoot,
		Remove
	} mode = Drag;

	struct CelesticBody ghostBody = {
		position: (Vector2){0, 0},
		velocity: (Vector2){0, 0},
		mass: 10,
		radius: cbrtf(40 / (3 * density * PI))
	};
	Vector2 positionOfCreation = (Vector2){0, 0};

	// gravity simulation
	bool pause = false;
	short unsigned int simulationSpeed = 1;

	// button colors
	Color dragIconColor = ORANGE;
	Color createIconColor = WHITE;
	Color removeIconColor = WHITE;

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

			if (mode != Create)
			{
				// find body in a cursor
				objectInCursor = false;
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

			if (mode == Create)
			{
				ghostBody.position = mousePositionInGrid;
			}

			if (mode == Shoot)
			{
				ghostBody.velocity = sDivide(10, minus(positionOfCreation, mousePosition));
			}

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				switch (mode)
				{
				case Drag:
					if (objectInCursor)
					{
						focus = true;
						bodyTarget = bodyFocus;
					}
					else
					{
						focus = false;
					}

					break;
				case Create:
					positionOfCreation = mousePosition;
					mode = Shoot;
					break;
				case Shoot:
					if (objectInCursor)
					{
						orbitBody(bodyFocus, ghostBody.position.x, ghostBody.position.y, ghostBody.mass * ghostBody.mass * ghostBody.mass);
					}
					else
					{
						newBody(ghostBody.position.x, ghostBody.position.y, ghostBody.velocity.x, ghostBody.velocity.y, ghostBody.mass * ghostBody.mass * ghostBody.mass);
					}
					mode = Create;
					objectInCursor = false;

					break;
				case Remove:
					if (objectInCursor)
					{
						removeBody(bodyFocus);
					}

					break;
				}
			}

			float wheel = GetMouseWheelMove();
			if (wheel)
			{
				if (mode == Create)
				{
					float mass = ghostBody.mass + 10 * wheel;

					if (mass > 1)
					{
						ghostBody.mass = mass;
						ghostBody.radius = cbrtf(4 * mass * mass * mass / (3 * density * PI));
					}
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
				if (mode != Drag && mousePosition.x >= 0 && mousePosition.x < screen.x / 3)
				{
					mode = Drag;
					dragIconColor = ORANGE;
					createIconColor = WHITE;
					removeIconColor = WHITE;
				}
				else if (mode != Create && mousePosition.x >= screen.x / 3 && mousePosition.x < 2 * screen.x / 3)
				{
					mode = Create;
					dragIconColor = WHITE;
					createIconColor = ORANGE;
					removeIconColor = WHITE;
				}
				else if (mode != Remove && mousePosition.x >= 2 * screen.x / 3 && mousePosition.x < screen.x)
				{
					mode = Remove;
					dragIconColor = WHITE;
					createIconColor = WHITE;
					removeIconColor = ORANGE;
				}

				// unfocus
				if (focus && mode != Drag)
					focus = false;
			}
		}

		// simulation
		if (!pause && bodyNumber > 0)
		{
			for (size_t time = 0; time < simulationSpeed; time++)
			{
				// forces
				for (size_t body = 0; body < bodyNumber; body++)
				{
					if (isnan(bodies[body].position.x) || isnan(bodies[body].position.x))
					{
						removeBody(body);
					}
					else
					{
						bodies[body].forces.x = 0;
						bodies[body].forces.y = 0;
					}
				}
				for (size_t i = 0; i < bodyNumber - 1; i++)
				{
					for (size_t j = i + 1; j < bodyNumber; j++)
					{
						Vector2 distance2D = minus(bodies[i].position, bodies[j].position);
						float distance = distance2D.x * distance2D.x + distance2D.y * distance2D.y;

						if (distance <= (bodies[i].radius + bodies[j].radius) * (bodies[i].radius + bodies[j].radius))
						{
							bodyCollision(i, j);
						}
						else
						{
							float bodyForce = gravityConstant * bodies[i].mass * bodies[j].mass / powf(distance, 1.5);

							bodies[i].forces = minus(bodies[i].forces, sTimes(bodyForce, distance2D));
							bodies[j].forces = plus(bodies[j].forces, sTimes(bodyForce, distance2D));
						}
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
		else if (mode == Shoot)
		{
			color = ORANGE;
		}

		if (objectInCursor)
		{
			DrawCircle(bodies[bodyFocus].position.x, bodies[bodyFocus].position.y, bodies[bodyFocus].radius + 2, color);
		}

		if (mode == Create)
		{
			DrawCircle(ghostBody.position.x, ghostBody.position.y, ghostBody.radius, YELLOW);
		}
		else if (mode == Shoot)
		{
			DrawLine(ghostBody.position.x, ghostBody.position.y, ghostBody.position.x - 10 * ghostBody.velocity.x, ghostBody.position.y - 10 * ghostBody.velocity.y, WHITE);
			DrawCircle(ghostBody.position.x, ghostBody.position.y, ghostBody.radius, GREEN);
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

		drawDragIcon((Vector2){screen.x / 4, screen.y - 25}, dragIconColor);
		drawCreateIcon((Vector2){2 * screen.x / 4, screen.y - 25}, createIconColor);
		drawRemoveIcon((Vector2){3 * screen.x / 4, screen.y - 25}, removeIconColor);

		DrawFPS(10, 10);
		if (pause)
			DrawText("paused", 10, 30, 20, GRAY);
		char speedText[16];
		snprintf(speedText, 16, "%ix", simulationSpeed);
		DrawText(speedText, 10, 50, 20, GRAY);

		if (debug)
		{
			char debug1[16];
			snprintf(debug1, 16, "%i", bodyTarget);
			DrawText(debug1, 10, 70, 20, GRAY);

			char debug2[16];
			snprintf(debug2, 16, "%i", objectInCursor);
			DrawText(debug2, 10, 90, 20, GRAY);

			char debug3[16];
			snprintf(debug3, 16, "%f", ghostBody.mass * ghostBody.mass * ghostBody.mass);
			DrawText(debug3, 10, 110, 20, GRAY);
			
			DrawText(debugText1, 10, 130, 20, GRAY);
			
			DrawText(debugText2, 10, 150, 20, GRAY);
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
