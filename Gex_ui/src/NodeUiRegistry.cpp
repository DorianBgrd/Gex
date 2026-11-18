#include "Gex_ui/include/Gex_ui/NodeUiRegistry.h"
#include "UiRes/uires.h"


Gex::Ui::NodeUiRegistry* Gex::Ui::NodeUiRegistry::registry = nullptr;


Gex::Ui::NodeUiRegistry* Gex::Ui::NodeUiRegistry::GetRegistry()
{
    if (!registry)
        registry = new NodeUiRegistry();

    return registry;
}


Gex::Ui::NodeUiRegistry::NodeUiRegistry():
    defaultColor(158, 158, 158)
{
    RegisterNodeResource(
            "CompoundInputs",
            Res::UiRes::GetRes()->GetQtAwesome()->icon(
                    fa::fa_solid, fa::fa_arrow_right_to_bracket
            ),
            QColor(247, 216, 106)
    );

    RegisterNodeResource(
            "CompoundOutputs",
            Res::UiRes::GetRes()->GetQtAwesome()->icon(
                    fa::fa_solid, fa::fa_arrow_right_from_bracket
            ),
            QColor(247, 216, 106)
    );
}


void Gex::Ui::NodeUiRegistry::RegisterNodeResource(
        const std::string& nodeType,
        const QIcon& icon,
        const QColor& color
)
{
    resources[nodeType] = {icon, color};
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


QColor Gex::Ui::NodeUiRegistry::GetNodeColor(
        const std::string& nodeType
)
{
    auto iter = resources.find(nodeType);
    if (iter == resources.end())
        return defaultColor;

    return iter->second.color;
}


Gex::Ui::NodeUiRegistry::NodeResource Gex::Ui::NodeUiRegistry::GetNodeResource(
        const std::string& nodeType
)
{
    auto iter = resources.find(nodeType);
    if (iter == resources.end())
        return {};

    return iter->second;
}