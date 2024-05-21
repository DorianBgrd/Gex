#include "LinearFunc.h"
#include "BezierFunc.h"




extern EXPORT RegisterUiPlugin(Gex::PluginLoader* loader)
{
    // Initialize ui plugin loader.
    auto* engine = UiTSys::UiTypeEngine::GetEngine();

    engine->RegisterType(typeid(Gex::InputRel::LinearFuncPtr).hash_code(),
                         "LinearFunc", new Gex::InputRel::LinearFuncInitWidgetCreator(),
                         new Gex::InputRel::LinearFuncWidgetCreator()
                         );

    engine->RegisterType(typeid(Gex::InputRel::BezierFuncPtr).hash_code(),
                         "BezierFunc", new Gex::InputRel::BezierFuncInitWidgetCreator(),
                         new Gex::InputRel::BezierFuncWidgetCreator());
}
