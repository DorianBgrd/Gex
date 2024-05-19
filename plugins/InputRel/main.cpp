#include "Func.h"
#include "Nodes.h"
#include "Gex/include/Gex.h"

#include "plugins/export.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    if (!Py_IsInitialized())
        Py_Initialize();

//    Gex::InputRel::Func::RegisterPythonWrapper();
//
//    Gex::InputRel::LinearFunc::RegisterPythonWrapper();

    loader->RegisterTypeHandler<Gex::InputRel::LinearFuncPtr ,
        Gex::InputRel::LinearFuncHandler>();

    loader->RegisterNode<Gex::InputRel::LinearRelBuilder>("InputRel/Linear");
}

