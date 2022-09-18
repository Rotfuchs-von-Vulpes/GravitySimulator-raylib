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
	return (Vector2)(Vector2Add(sDivide(zoom, Vector2Subtract(screenPostion, sDivide(2, screen))), gridCenter));
}

float squareDistance(Vector2 u, Vector2 v)
{
	return (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
}