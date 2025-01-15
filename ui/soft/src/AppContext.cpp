#include "ui/soft/include/AppContext.h"


template<typename T>
inline boost::python::list ToPyList(std::vector<T> vec)
{
    boost::python::list pyList;

    for (const T& v : vec)
    {
        pyList.append(v);
    }

    return pyList;
}


template<typename T>
inline std::vector<T> ToVector(boost::python::list pyList)
{
    std::vector<T> cppVec;

    for (unsigned int i = 0; i < boost::python::len(pyList); i++)
    {
        const T& w = boost::python::extract<T>(pyList[i]);

        cppVec.push_back(w);
    }

    return cppVec;
}



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


bool SoftApi::SelectionContext::registered = false;
SoftApi::SelectionContext* SoftApi::SelectionContext::instance = nullptr;


SoftApi::SelectionContext* SoftApi::SelectionContext::GetInstance()
{
    if (!instance)
        instance = new SelectionContext();

    return instance;
}


void SoftApi::SelectionContext::Select(
        const Gex::NodeWkList& selection)
{
    auto* inst = GetInstance();
    inst->currentSelection = selection;

    inst->PushSelection(selection);
}


void SoftApi::SelectionContext::PushSelection(
        const Gex::NodeWkList& selection)
{
    for (const auto& cb : callbacks)
    {
        cb.second(selection);
    }
}


Gex::NodeWkList SoftApi::SelectionContext::Selection()
{
    return GetInstance()->currentSelection;
}


Gex::CallbackId  SoftApi::SelectionContext::Connect(
        const SelectionCallback& callback)
{
    auto* inst = GetInstance();

    inst->currentId += 1;

    inst->callbacks[inst->currentId] = callback;

    return inst->currentId;
}


bool SoftApi::SelectionContext::Disconnect(Gex::CallbackId id)
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


boost::python::object SelCtx_Select(boost::python::tuple args,
                                    boost::python::dict kwargs)
{
    boost::python::list sel(args[0]);

    std::vector<Gex::NodeWkPtr> csel;
    for (unsigned int i = 0; i < boost::python::len(sel); i++)
    {
        boost::python::object pyObj = sel[i];
        boost::python::extract<const Gex::NodePtr&> ex1(pyObj);
        if (ex1.check())
        {
            csel.emplace_back(ex1());
            continue;
        }

        boost::python::extract<const Gex::NodeWkPtr&> ex2(pyObj);
        if (ex2.check())
        {
            csel.push_back(ex2());
            continue;
        }

        const std::string t = boost::python::extract<const std::string>(pyObj.attr("__class__"));
        std::cerr << "Failed converting object of type " << t << std::endl;
    }

    SoftApi::SelectionContext::Select(csel);

    return {};
}


boost::python::object SelCtx_Selection(boost::python::tuple args,
                                    boost::python::dict kwargs)
{
    return ToPyList(SoftApi::SelectionContext::Selection());
}


boost::python::object SelCtx_Connect(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    boost::python::object callable = args[0];

    return boost::python::object(
            SoftApi::SelectionContext::Connect(
                [callable](const Gex::NodeWkList& selection)
                {
                    callable(ToPyList(selection));
                }
        )
    );
}


void SoftApi::SelectionContext::RegisterPythonWrapper()
{
    if (registered)
        return;

    boost::python::class_<SelectionContext>("SelectionContext", boost::python::no_init)
            .def("Select", boost::python::raw_function(&SelCtx_Select, 1))
            .staticmethod("Select")
            .def("Selection", boost::python::raw_function(SelCtx_Selection, 0))
            .staticmethod("Selection")
            .def("Connect", boost::python::raw_function(SelCtx_Connect, 1))
            .staticmethod("Connect")
            .def("Disconnect", &SelectionContext::Disconnect)
            .staticmethod("Disconnect")
            ;

    registered = true;
}
