#include "Gex/include/utils.h"
#include "Gex/include/Config.h"
#include "Gex/include/Node.h"

#include <stdlib.h>


Gex::AttributePtr Gex::Utils::CopyAttribute(
        const AttributePtr& attr,
        const NodePtr& destinationNode,
        std::function<void(AttributePtr)> editor
)
{
    auto copy = attr->Copy(attr->Name(), destinationNode);

    if (editor)
    {
        editor(copy);
    }

    if (attr->IsHolder())
    {
        for (const auto& childAttribute : attr->GetChildAttributes())
        {
            auto childCopy = CopyAttribute(
                childAttribute.ToShared(),
                destinationNode, editor
            );

            copy->AddChildAttribute(childCopy);
        }
    }

    return copy;
}


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
    for (const auto& n : nodes)
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


std::string Gex::Utils::GetEnv(const std::string& name)
{
    size_t envSize;
    const char* envName = name.c_str();

    getenv_s( &envSize, nullptr, 0, envName);
    if (!envSize)
        return {};

    char* envResult = (char*) malloc(envSize * sizeof(char));
    getenv_s(&envSize, envResult, envSize, envName);

    return envResult;
}

