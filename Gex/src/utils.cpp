#include "Gex/include/utils.h"
#include "Gex/include/Config.h"
#include "Gex/include/Node.h"


std::string Gex::Utils::ValidateName(const std::string& name)
{
    size_t p = std::string::npos;

    std::string result = name;
    std::string sep = Gex::Config::GetConfig().nodePathSeparator;
    while ((p = result.find(sep)) != std::string::npos)
    {
        result.replace(p, 1, "_");
    }

    return result;
}


std::string Gex::Utils::UniqueName(const std::string &name, const Gex::NodeList& nodes)
{
    std::string uniqueName = Gex::Utils::ValidateName(name);

    int counter = 1;
    std::vector<std::string> nodeNames;
    for (auto n : nodes)
    {
        nodeNames.push_back(n->Name());
    }

    while (std::find(nodeNames.begin(), nodeNames.end(), uniqueName) != nodeNames.end())
    {
        uniqueName = name + std::to_string(counter);
        counter++;
    }

    return uniqueName;
}