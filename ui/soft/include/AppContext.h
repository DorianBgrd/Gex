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


    typedef std::function<void(const Gex::NodeWkList&)> SelectionCallback;


    class SOFT_API SelectionContext
    {
        static bool registered;
        static SelectionContext* instance;

        Gex::CallbackId currentId = 0;
        std::map<Gex::CallbackId, SelectionCallback> callbacks;
        Gex::NodeWkList currentSelection;
    private:
        static SelectionContext* GetInstance();

    public:
        static void Select(const Gex::NodeWkList& selection);

        static Gex::NodeWkList Selection();

    private:
        void PushSelection(const Gex::NodeWkList& selection);

    public:
        static Gex::CallbackId Connect(const SelectionCallback& callback);

        static bool Disconnect(Gex::CallbackId id);

    public:
        static void RegisterPythonWrapper();
    };
}


#endif //GEX_APPCONTEXT_H
