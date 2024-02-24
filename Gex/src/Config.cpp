#include "Gex/include/Config.h"

Gex::Config Gex::Config::globalConfig;


Gex::Config Gex::Config::GetConfig()
{
    return globalConfig;
};