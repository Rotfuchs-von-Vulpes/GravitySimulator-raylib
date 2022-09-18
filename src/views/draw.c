#include "raylib.h"

void draw(Camera2D camera)
{
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

	for (size_t body = 0; body < bodyCount; body++)
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

	EndDrawing();
}