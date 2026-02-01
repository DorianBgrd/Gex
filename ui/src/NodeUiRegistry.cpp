#include "ui/include/NodeUiRegistry.h"
#include "UiRes/include/uires.h"


Gex::Ui::NodeUiRegistry* Gex::Ui::NodeUiRegistry::registry = nullptr;


Gex::Ui::NodeUiRegistry* Gex::Ui::NodeUiRegistry::GetRegistry()
{
    if (!registry)
        registry = new NodeUiRegistry();

    return registry;
}


Gex::Ui::NodeUiRegistry::NodeUiRegistry()
{
    RegisterNodeIcon(
            "CompoundInputs",
            Res::UiRes::GetRes()->GetQtAwesome()->icon(
                    fa::fa_solid, fa::fa_arrow_right_to_bracket
            )
    );

    RegisterNodeIcon(
            "CompoundOutputs",
            Res::UiRes::GetRes()->GetQtAwesome()->icon(
                    fa::fa_solid, fa::fa_arrow_right_from_bracket
            )
    );
}


void Gex::Ui::NodeUiRegistry::RegisterNodeIcon(
        const std::string& nodeType,
        const QIcon& icon
)
{
    resources[nodeType] = {icon};
}


QIcon Gex::Ui::NodeUiRegistry::GetNodeIcon(
        const std::string& nodeType
)
{
    auto iter = resources.find(nodeType);
    if (iter == resources.end())
        return {};

    return iter->second.icon;
}