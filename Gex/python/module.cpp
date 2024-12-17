#include "Gex/include/Attribute_Wrap.h"
#include "Gex/include/Graph_Wrap.h"
#include "Gex/include/Status_Wrap.h"
#include "Gex/include/Node_Wrap.h"
#include "Gex/include/io_Wrap.h"
#include "Gex/include/PluginLoader_Wrap.h"
#include "Gex/include/NodeFactory_Wrap.h"
#include "Gex/include/References_Wrap.h"
#include "Gex/include/UndoStack_Wrap.h"

#include "boost/python.hpp"


BOOST_PYTHON_MODULE(Gex_Python)
{
    Gex::Python::Feedback_Wrap::RegisterPythonWrapper();

    Gex::Python::Attribute_Wrap::RegisterPythonWrapper();

    Gex::Python::GraphContext_Wrap::RegisterPythonWrapper();

    Gex::Python::Node_Wrap::RegisterPythonWrapper();

    Gex::Python::CompoundNode_Wrap::RegisterPythonWrapper();

    Gex::Python::PluginLoader_Wrap::RegisterPythonWrapper();

    Gex::Python::NodeBuilder_Wrap::RegisterPythonWrapper();

    Gex::Python::NodeFactory_Wrap::RegisterPythonWrapper();

    Gex::Python::References_Wrap::RegisterPythonWrapper();

    Gex::Python::IO::RegisterPythonWrapper();

    Gex::Python::UndoStack_Wrap::RegisterPythonWrapper();
}