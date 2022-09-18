bool pause = false;
short unsigned int simulationSpeed = 1;

void simulation(void)
{
  for (size_t time = 0; time < simulationSpeed; time++)
  {
    // forces
    for (size_t body = 0; body < bodyCount; body++)
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

    for (size_t i = 0; i < bodyCount - 1; i++)
    {
      for (size_t j = i + 1; j < bodyCount; j++)
      {
        Vector2 distance2D = Vector2Subtract(bodies[i].position, bodies[j].position);
        float distance = Vector2DistanceSqr(bodies[i].position, bodies[j].position);

        if (distance <= (bodies[i].radius + bodies[j].radius) * (bodies[i].radius + bodies[j].radius))
        {
          bodyCollision(i, j);
        }
        else
        {
          float bodyForce = gravityConstant * bodies[i].mass * bodies[j].mass / powf(distance, 1.5);

          bodies[i].forces = Vector2Subtract(bodies[i].forces, sTimes(bodyForce, distance2D));
          bodies[j].forces = Vector2Add(bodies[j].forces, sTimes(bodyForce, distance2D));
        }
      }
    }

    // movimenty
    for (size_t body = 0; body < bodyCount; body++)
    {
      bodies[body].velocity = Vector2Add(bodies[body].velocity, sDivide(bodies[body].mass, bodies[body].forces));
      bodies[body].position = Vector2Add(bodies[body].position, bodies[body].velocity);
    }
  }
}