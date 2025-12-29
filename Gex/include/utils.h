#ifndef GEX_UTILS_H
#define GEX_UTILS_H

#include <string>
#include <functional>
#include "defs.h"

namespace Gex::Utils
{
    AttributePtr CopyAttribute(const AttributePtr& attr,
                               const NodePtr& destinationNode,
                               std::function<void(AttributePtr)> editor=nullptr);

    std::string ValidateName(const std::string& name);

    std::string UniqueName(const std::string& name, const NodeList& nodes);

    std::string GetEnv(const std::string& name);
}

#endif //GEX_UTILS_H
