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