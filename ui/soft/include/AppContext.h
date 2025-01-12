#ifndef GEX_APPCONTEXT_H
#define GEX_APPCONTEXT_H

#include "Gex/include/Gex.h"
#include "MainWindow.h"
#include "ui/include/api.h"
#include "softApi.h"


namespace SoftApi
{
    class SOFT_API AppContext
    {
        static bool registered;
        static Gex::Editor::MainWindow* mainWindow;

    public:
        static void SetMainWindow(Gex::Editor::MainWindow* mainWindow);

        static Gex::Editor::MainWindow* GetMainWindow();

        static Gex::NodePtr CurrentNode();

        static void RegisterPythonWrapper();
    };
}


#endif //GEX_APPCONTEXT_H
