#include "Tsys/tsys.h"
#include "Tsys/defaultTypes.h"


TSys::TypeValueRegistry* TSys::GetRegistry()
{
    return TypeValueRegistry::GetInstance(
            &TSys::RegisterDefaultTypes
    );
}