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

    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
}

#endif //GEX_UTILS_H
