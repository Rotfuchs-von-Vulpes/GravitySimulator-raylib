typedef struct
{
	Vector2 position;
	Vector2 velocity;
	Vector2 forces;
	float mass;
	float radius;
} CelesticBody;

unsigned int bodyCapacity = 5;
unsigned int bodyCount = 0;
CelesticBody* bodies = NULL;

char debugText1[16] = "";
char debugText2[16] = "";

void newBody(float x, float y, float vx, float vy, float m)
{
	bodies[bodyCount].position.x = x;
	bodies[bodyCount].position.y = y;
	bodies[bodyCount].velocity.x = vx;
	bodies[bodyCount].velocity.y = vy;
	bodies[bodyCount].mass = m;
	bodies[bodyCount].radius = cbrtf(4 * m / (3 * density * PI));

	if (bodyCount++ >= bodyCapacity - 1)
	{
		bodyCapacity *= 2;
		bodies = (CelesticBody*) realloc(bodies, sizeof(CelesticBody) * bodyCapacity);
	}
}

void removeBody(unsigned int index)
{
	if (index == bodyCount)
	{
		bodyCount--;
	}
	else if (index < bodyCount)
	{
		bodies[index] = bodies[--bodyCount];
	}
}

void orbitBody(unsigned int index, float x, float y, float m)
{
	CelesticBody body = bodies[index];

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
	CelesticBody body1 = bodies[index1];
	CelesticBody body2 = bodies[index2];

	removeBody(index2);
	removeBody(index1);

	float x = (body1.mass * body1.position.x + body2.mass * body2.position.x) / (body1.mass + body2.mass);
	float y = (body1.mass * body1.position.y + body2.mass * body2.position.y) / (body1.mass + body2.mass);
	float vx = (body1.mass * body1.velocity.x + body2.mass * body2.velocity.x) / (body1.mass + body2.mass);
	float vy = (body1.mass * body1.velocity.y + body2.mass * body2.velocity.y) / (body1.mass + body2.mass);

	float m = body1.mass + body2.mass;

	newBody(x, y, vx, vy, m);
}