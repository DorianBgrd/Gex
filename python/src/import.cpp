#include "python/include/Attribute.h"
#include "python/include/Graph.h"
#include "python/include/Status.h"
#include "python/include/Node.h"
#include "python/include/io.h"
#include "python/include/PluginLoader.h"

#include "boost/python.hpp"


BOOST_PYTHON_MODULE(Gex_Python)
{
    Gex::Python::Feedback_Wrap::RegisterPythonWrapper();

    Gex::Python::Attribute_Wrap::RegisterPythonWrapper();

    Gex::Python::Graph_Wrap::RegisterPythonWrapper();

    Gex::Python::GraphContext_Wrap::RegisterPythonWrapper();

    Gex::Python::Node_Wrap::RegisterPythonWrapper();

    Gex::Python::CompoundNode_Wrap::RegisterPythonWrapper();

    Gex::Python::PluginLoader_Wrap::RegisterPythonWrapper();

    Gex::Python::IO::RegisterPythonWrapper();

//    Gex::Python::GraphContext_Wrap()::
}