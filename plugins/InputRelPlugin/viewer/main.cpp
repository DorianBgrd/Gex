#include "FuncViewer.h"
#include "LinearFunc.h"
//#include "BezierFunc.h"
#include "ui/include/PluginLoader.h"

#include "Viewer.h"



extern EXPORT RegisterUiPlugin(Gex::Ui::UiPluginLoader* loader)
{
    // Initialize ui plugin loader.
    Gex::InputRel::FuncDelegateFactory::RegisterDelegate
            <Gex::InputRel::LinearFuncPtr, Gex::InputRel::LinearFuncDelegateCreator>();

    loader->RegisterNodeViewer<Gex::InputRel::FuncNodeViewerCreator>(
            "InputRel/Linear");
}
