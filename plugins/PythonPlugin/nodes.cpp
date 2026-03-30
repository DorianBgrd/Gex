#include "nodes.h"
#include "Tsys/defaultTypes.h"

#include "Gex/include/Gex.h"
#include "Gex/include/Graph_Wrap.h"
#include "Gex/include/Wrappers.h"

#include "expression.h"


void PythonPlugin::Nodes::Expression::InitAttributes()
{
    CreateAttribute<std::string>(
            "code",
            Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );
}


bool PythonPlugin::Nodes::Expression::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    if (!Py_IsInitialized())
    {
        Py_Initialize();
    }

    Python::Expression expr(
            context.GetAttribute("code")
            .GetValue<std::string>()
    );

    if (!expr.Execute(context))
    {
        return false;
    }

    return true;
}
