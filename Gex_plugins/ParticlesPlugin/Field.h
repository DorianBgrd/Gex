#ifndef GEX_FIELD_H
#define GEX_FIELD_H

#include "Gex_plugins/InputRelPlugin/Func.h"

#include "Particles.h"


namespace Gex::Particles
{
    class Field
    {
        InputRel::Func attenuation;

        void Apply(Particle* particle) const;
    };
}

#endif //GEX_FIELD_H
