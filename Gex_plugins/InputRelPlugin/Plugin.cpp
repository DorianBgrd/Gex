#include "Func.h"
#include "BezierFunc.h"
#include "Nodes.h"
#include "Gex/include/Gex/Gex.h"

#include "Gex_plugins/export.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    if (!Py_IsInitialized())
        Py_Initialize();

//    Gex::InputRel::Func::RegisterPythonWrapper();
//
//    Gex::InputRel::LinearFunc::RegisterPythonWrapper();

    loader->GetRegistry()->RegisterType<Gex::InputRel::LinearFuncPtr>(
            "LinearFunc", TSys::DefaultCreator,
            Gex::InputRel::SerializeFunc<Gex::InputRel::LinearFunc>,
            Gex::InputRel::DeserializeFunc<Gex::InputRel::LinearFunc>
    );

    loader->GetRegistry()->RegisterType<Gex::InputRel::BezierFuncPtr>(
            "BezierFunc", TSys::DefaultCreator,
            Gex::InputRel::SerializeFunc<Gex::InputRel::BezierFunc>,
            Gex::InputRel::DeserializeFunc<Gex::InputRel::BezierFunc>
    );

    loader->RegisterNode<Gex::InputRel::LinearRelBuilder>("InputRel/Linear");

    loader->RegisterNode<Gex::InputRel::BezierRelBuilder>("InputRel/Bezier");
}

