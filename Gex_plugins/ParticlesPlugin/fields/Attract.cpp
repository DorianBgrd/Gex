#include "Attract.h"


Gex::Particles::AttractionField::AttractionField(Vector p)
{
    position = p;
}


void Gex::Particles::AttractionField::Solve(Particle* particle)
{
    Vector toPosition = {position.x - particle->position.x,
                         position.y - particle->position.y,
                         position.z - particle->position.z};

    toPosition *= particle->velocity.Length();

    double match = 0;
    if (particle->mass > 0)
    {
        match = 1 / particle->mass;
    }

    double dx = toPosition.x - particle->velocity.x;
    double dy = toPosition.y - particle->velocity.y;
    double dz = toPosition.z - particle->velocity.z;

    particle->velocity.x += (match * dx);
    particle->velocity.y += (match * dy);
    particle->velocity.z += (match * dz);
}