#ifndef GEX_DIRECTION_H
#define GEX_DIRECTION_H

#include "Gex_plugins/Particles/System.h"

namespace Gex::Particles
{
    class DirectionField: Field
    {
        Vector direction;
        double factor = 1;
    public:
        DirectionField(Vector direction,
                       double factor);

        void Solve(Particle* particle) override;
    };
}

#endif //GEX_DIRECTION_H
