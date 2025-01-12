#include "ui/soft/include/AppContext.h"


bool SoftApi::AppContext::registered = false;
Gex::Editor::MainWindow* SoftApi::AppContext::mainWindow = nullptr;


void SoftApi::AppContext::SetMainWindow(
        Gex::Editor::MainWindow* w)
{
    mainWindow = w;
}


Gex::Editor::MainWindow* SoftApi::AppContext::GetMainWindow()
{
    return mainWindow;
}


Gex::NodePtr SoftApi::AppContext::CurrentNode()
{
    if (!mainWindow)
        return {};

    return mainWindow->CurrentNode();
}


void SoftApi::AppContext::RegisterPythonWrapper()
{
    if (registered)
        return;

    boost::python::class_<AppContext>("AppContext", boost::python::no_init)
            .def("CurrentNode", &AppContext::CurrentNode)
            .staticmethod("CurrentNode")
            ;

    registered = true;
}