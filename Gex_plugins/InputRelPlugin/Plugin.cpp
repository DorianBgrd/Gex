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
            Gex::InputRel::DeserializeFunc<Gex::InputRel::LinearFunc>,
            TSys::UnuseFromPython, TSys::UnuseToPython,
            TSys::UseDefaultCtor<Gex::InputRel::LinearFunc>,
            nullptr, nullptr, nullptr, nullptr, nullptr

    );

    loader->GetRegistry()->RegisterType<Gex::InputRel::BezierFuncPtr>(
            "BezierFunc", TSys::DefaultCreator,
            Gex::InputRel::SerializeFunc<Gex::InputRel::BezierFunc>,
            Gex::InputRel::DeserializeFunc<Gex::InputRel::BezierFunc>,
            TSys::UnuseFromPython, TSys::UnuseToPython,
            TSys::UseDefaultCtor<Gex::InputRel::BezierFunc>,
            nullptr, nullptr, nullptr, nullptr, nullptr
    );

    loader->RegisterNode<Gex::InputRel::LinearRelBuilder>("InputRel/Linear");

    loader->RegisterNode<Gex::InputRel::BezierRelBuilder>("InputRel/Bezier");
}

