#include "Viewer.h"

#include "FuncViewer.h"


void Gex::InputRel::FuncNodeViewer::Setup()
{
    funcView = new FuncViewWidget(this);

    auto* layout = new QVBoxLayout();
    setLayout(layout);

    layout->addWidget(funcView);
}


void Gex::InputRel::FuncNodeViewer::OnNodeUpdated(Gex::NodeWkPtr node)
{
    if (!node)
        return;

    auto attribute = node->GetAttribute("Func");
    if (!attribute)
        return;

    funcView->SetFunction(attribute->GetAnyValue());

    auto inputAttribute = node->GetAttribute("Input");
    if (!attribute)
        return;

    funcView->SetTick(inputAttribute->Get<double>());
}


void Gex::InputRel::FuncNodeViewer::OnAttributeUpdated(
        const AttributePtr& attribute,
        const AttributeChange& change)
{
    if (attribute->Name() == "Input")
    {
        funcView->SetTick(attribute->Get<double>());
    }
}
