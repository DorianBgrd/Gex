#ifndef TSYS_DEFAULT_TYPES
#define TSYS_DEFAULT_TYPES

#include "api.h"


namespace TSys
{
    class TypeValueRegistry;

    void RegisterDefaultTypes(TypeValueRegistry* registry);
}

#endif //TSYS_DEFAULT_TYPES