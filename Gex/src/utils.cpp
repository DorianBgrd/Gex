#include "Gex/include/utils.h"
#include "Gex/include/Config.h"


std::string Gex::Utils::ValidateName(const std::string& name)
{
    size_t p = std::string::npos;

    std::string result = name;
    std::string sep = Gex::Config::GetConfig().nodePathSeparator;
    while ((p = name.find(sep)) != std::string::npos)
    {
        result.replace(p, 1, "_");
    }

    return result;
}