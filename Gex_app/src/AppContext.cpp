#include "Gex_app/include/AppContext.h"


template<typename T>
inline pybind11::list ToPyList(std::vector<T> vec)
{
    pybind11::list pyList;

    for (const T& v : vec)
    {
        pyList.append(pybind11::cast(v));
    }

    return pyList;
}


template<typename T>
inline std::vector<T> ToVector(pybind11::list pyList)
{
    std::vector<T> cppVec;

    for (unsigned int i = 0; i < pybind11::len(pyList); i++)
    {
        const T& w = pyList[i].cast<T>();

        cppVec.push_back(w);
    }

    return cppVec;
}



bool Gex::App::AppContext::registered = false;
Gex::App::MainWindow* Gex::App::AppContext::mainWindow = nullptr;


void Gex::App::AppContext::SetMainWindow(
        Gex::App::MainWindow* w)
{
    mainWindow = w;
}


Gex::App::MainWindow* Gex::App::AppContext::GetMainWindow()
{
    return mainWindow;
}


Gex::NodePtr Gex::App::AppContext::CurrentNode()
{
    if (!mainWindow)
        return {};

    return mainWindow->CurrentNode();
}


void Gex::App::AppContext::RegisterPythonWrapper(pybind11::module_& mod)
{
    if (registered)
        return;

    pybind11::class_<AppContext>(mod, "AppContext")
            .def_static("CurrentNode", &AppContext::CurrentNode)
            ;

    registered = true;
}


bool Gex::App::SelectionContext::registered = false;
Gex::App::SelectionContext* Gex::App::SelectionContext::instance = nullptr;


Gex::App::SelectionContext* Gex::App::SelectionContext::GetInstance()
{
    if (!instance)
        instance = new SelectionContext();

    return instance;
}


void Gex::App::SelectionContext::Select(
        const Gex::NodeWkList& selection)
{
    auto* inst = GetInstance();
    inst->currentSelection = selection;

    inst->PushSelection(selection);
}


void Gex::App::SelectionContext::PushSelection(
        const Gex::NodeWkList& selection)
{
    for (const auto& cb : callbacks)
    {
        cb.second(selection);
    }
}


Gex::NodeWkList Gex::App::SelectionContext::Selection()
{
    return GetInstance()->currentSelection;
}


Gex::CallbackId  Gex::App::SelectionContext::Connect(
        const SelectionCallback& callback)
{
    auto* inst = GetInstance();

    inst->currentId += 1;

    inst->callbacks[inst->currentId] = callback;

    return inst->currentId;
}


bool Gex::App::SelectionContext::Disconnect(Gex::CallbackId id)
{
    auto* inst = GetInstance();

    auto iter = inst->callbacks.find(id);

    if (iter == inst->callbacks.end())
    {
        return false;
    }

    inst->callbacks.erase(iter);
    return true;
}


pybind11::object SelCtx_Select(pybind11::args args,
                               pybind11::kwargs kwargs)
{
    pybind11::list sel(args[0]);

    std::vector<Gex::NodeWkPtr> csel;
    for (unsigned int i = 0; i < pybind11::len(sel); i++)
    {
        pybind11::object pyObj = sel[i];
        auto ex1 = pyObj.cast<const Gex::NodePtr&>();
        if (ex1)
        {
            csel.emplace_back(ex1);
            continue;
        }

        auto ex2 = pyObj.cast<const Gex::NodeWkPtr&>();
        if (ex2)
        {
            csel.push_back(ex2);
            continue;
        }

        const std::string t = pyObj.attr("__class__").cast<const std::string>();
        std::cerr << "Failed converting object of type " << t << std::endl;
    }

    Gex::App::SelectionContext::Select(csel);

    return {};
}


pybind11::object SelCtx_Selection(pybind11::args args,
                                  pybind11::kwargs kwargs)
{
    return ToPyList(Gex::App::SelectionContext::Selection());
}


pybind11::object SelCtx_Connect(
        pybind11::args args,
        pybind11::kwargs kwargs
)
{
    pybind11::object callable = args[0];

    return pybind11::cast(
            Gex::App::SelectionContext::Connect(
                [callable](const Gex::NodeWkList& selection)
                {
                    callable(ToPyList(selection));
                }
        )
    );
}


void Gex::App::SelectionContext::RegisterPythonWrapper(pybind11::module_& mod)
{
    if (registered)
        return;

    pybind11::class_<SelectionContext>(mod, "SelectionContext")
            .def_static("Select", &SelectionContext::Select)
            .def_static("Selection", &SelectionContext::Selection)
            .def_static("Connect", &SelCtx_Connect)
            .def_static("Disconnect", &SelectionContext::Disconnect)
            ;

    registered = true;
}
