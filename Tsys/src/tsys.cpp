#include "Tsys/tsys.h"
#include "Tsys/typeSystem.h"
#include "Tsys/defaultTypes.h"


TSys::TypeValueRegistry* TSys::GetRegistry()
{
    return TypeValueRegistry::Get(
            &TSys::RegisterDefaultTypes
    );
}