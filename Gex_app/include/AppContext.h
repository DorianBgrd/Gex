#ifndef GEX_APPCONTEXT_H
#define GEX_APPCONTEXT_H

#include "Gex/include/Gex/Gex.h"
#include "MainWindow.h"
#include "Gex_ui/include/api.h"
#include "softApi.h"


namespace Gex::App
{
    class SOFT_API AppContext
    {
        static bool registered;
        static Gex::App::MainWindow* mainWindow;

    public:
        static void SetMainWindow(MainWindow* mainWindow);

        static MainWindow* GetMainWindow();

        static Gex::NodePtr CurrentNode();

        static void RegisterPythonWrapper(pybind11::module_& mod);
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
        static void RegisterPythonWrapper(pybind11::module_& mod);
    };
}


#endif //GEX_APPCONTEXT_H
