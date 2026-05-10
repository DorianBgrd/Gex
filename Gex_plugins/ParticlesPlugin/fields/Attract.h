#ifndef GEX_ATTRACT_H
#define GEX_ATTRACT_H

#include "Gex_plugins/Particles/System.h"

namespace Gex::Particles
{
    class AttractionField: Field
    {
        Vector position;
    public:
        AttractionField(Vector position);

        void Solve(Particle* particle) override;
    };
}

#endif //GEX_ATTRACT_H
