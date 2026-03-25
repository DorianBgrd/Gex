#include "Gex/include/Gex.h"
#include "nodes.h"

#include "../export.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
//    loader->RegisterNode<PythonPlugin::Nodes::PythonNodeBuilder>("Python/PythonNode");

    loader->RegisterNode<PythonPlugin::Nodes::ExpressionBuilder>("Python/Expression");
}