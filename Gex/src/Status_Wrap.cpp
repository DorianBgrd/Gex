#include "Gex/include/Gex/Status_Wrap.h"
#include "Gex/include/Gex/Status.h"


bool Gex::Python::Feedback_Wrap::pythonRegistered = false;
Gex::Python::PyClassRegistry Gex::Python::Feedback_Wrap::registry;



pybind11::object StatusGet(pybind11::args args,
                                pybind11::kwargs kwargs)
{
    Gex::Feedback* feedback = args[0].cast<Gex::Feedback*>();
    return pybind11::cast(feedback->status);
}


pybind11::object StatusSet(pybind11::args args,
                                pybind11::kwargs kwargs)
{
    Gex::Feedback* feedback = args[0].cast<Gex::Feedback*>();
    Gex::Status st = args[1].cast<Gex::Status>();

    feedback->status = st;

    return {};
}


pybind11::object MessageGet(pybind11::args args,
                            pybind11::kwargs kwargs)
{
    auto* feedback = args[0].cast<Gex::Feedback*>();
    return pybind11::cast(feedback->message);
}


pybind11::object MessageSet(pybind11::args args,
                            pybind11::kwargs kwargs)
{
    Gex::Feedback* feedback = args[0].cast<Gex::Feedback*>();
    std::string st = args[1].cast<std::string>();

    feedback->message = st;

    return pybind11::none();
}


bool Gex::Python::Feedback_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                       PyThreadState* state)
{
    if (registry.IsRegistered(state))
        return false;

    pybind11::enum_<Gex::Status>(mod, "Status", pybind11::module_local(false))
            .value("None", Gex::Status::None)
            .value("Success", Gex::Status::Success)
            .value("Warning", Gex::Status::Warning)
            .value("Failed", Gex::Status::Failed)
            .value("Error", Gex::Status::Error)
            ;


    pybind11::class_<Gex::Feedback>(mod, "Feedback", pybind11::module_local(false))
            .def(pybind11::init<>())
            .def_property("status", &StatusGet, &StatusSet)
            .def_property("message", &MessageGet, &MessageSet)
            ;

    return registry.Register(state);
}


bool Gex::Python::Feedback_Wrap::IsRegistered()
{
    return pythonRegistered;
}