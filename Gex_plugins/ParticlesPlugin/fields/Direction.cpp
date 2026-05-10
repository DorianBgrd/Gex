#include "Direction.h"


Gex::Particles::DirectionField::DirectionField(
        Vector d, double f)
{
    direction = d;
    factor = f;
}


void Gex::Particles::DirectionField::Solve(Particle* particle)
{
    particle.velocity += (direction * factor);
}
