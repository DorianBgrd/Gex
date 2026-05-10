#ifndef GEX_SPIN_H
#define GEX_SPIN_H

#include "Gex_plugins/Particles/System.h"

namespace Gex::Particles
{
    class SpinField: Field
    {
        Vector center;
    public:
        SpinField(Vector center);

        void Solve(Particle* particle) override;
    };
}

#endif //GEX_SPIN_H
