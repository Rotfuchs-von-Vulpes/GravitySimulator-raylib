#include "raylib.h"
#include "raymath.h"

// modes rules
enum modes
{
	Drag,
	Create,
	Shoot,
	Remove
} mode = Drag;

CelesticBody ghostBody = {
		.position = {0, 0},
		.velocity = {0, 0},
		.mass = 10,
		.radius = cbrtf(40 / (3 * density * PI))};
Vector2 positionOfCreation = {0, 0};

// UI
Color dragIconColor = ORANGE;
Color createIconColor = WHITE;
Color removeIconColor = WHITE;

const Vector2 screen = {800, 450};

// mouse
Vector2 wherePressed = {0, 0};
bool canDrag = true;
Vector2 mousePosition = {-100.0f, -100.0f};
size_t bodyTarget = 0, bodyFocus = 0;
bool objectInCursor = false;
Vector2 target = {0, 0};
bool focus = false;

Camera2D inputs(Camera2D camera)
{ 
	// keyboard
	if (IsKeyPressed(KEY_SPACE))
		pause = !pause;
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
			Vector2 draged = sDivide(camera.zoom, Vector2Subtract(target, mousePosition));
			camera.target = Vector2Add(wherePressed, draged);
		}
		if (IsMouseButtonUp(MOUSE_RIGHT_BUTTON))
		{
			canDrag = false;
		}

		if (mode != Create)
		{
			// find body in a cursor
			objectInCursor = false;
			for (size_t body = 0; body < bodyCount; body++)
			{
				if (Vector2DistanceSqr(bodies[body].position, mousePositionInGrid) < bodies[body].radius * bodies[body].radius)
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
			ghostBody.velocity = sDivide(10, Vector2Subtract(positionOfCreation, mousePositionInGrid));
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
				positionOfCreation = mousePositionInGrid;
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

	return camera;
}