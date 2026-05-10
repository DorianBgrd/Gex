#ifndef GEX_SOLVER_H
#define GEX_SOLVER_H

#include <vector>
#include "Field.h"
#include "Particles.h"

namespace Gex::Particles
{
    class ParticleSolver
    {
        std::vector<Field*> fields;

        void Solve(Particles* particles, float time);
    };
}

#endif //GEX_SOLVER_H
