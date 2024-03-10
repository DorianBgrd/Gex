#ifndef GEX_UTILS_H
#define GEX_UTILS_H

#include <string>
#include "defs.h"

namespace Gex::Utils
{
    std::string ValidateName(const std::string& name);

    std::string UniqueName(const std::string& name, const NodeList& nodes);
}

#endif //GEX_UTILS_H
