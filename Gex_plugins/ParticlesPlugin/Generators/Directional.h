#ifndef GEX_DIRECTIONAL_H
#define GEX_DIRECTIONAL_H

#include "Gex_plugins/Particles/System.h"

namespace Gex::Particles
{
    class DirectionalGenerator: public Generator
    {
        float particlePerUpdate=0;

    public:
        DirectionalGenerator(float particlePerUpdate);

        ParticleList Generate() const override;
    };
}

#endif //GEX_DIRECTIONAL_H
