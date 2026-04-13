#include "Gex/include/Profiler_Wrap.h"


bool Gex::Python::Event_Wrap::registered = false;
Gex::Python::PyClassRegistry Gex::Python::Event_Wrap::registry;

bool Gex::Python::EvaluationProfiler_Wrap::registered = false;
Gex::Python::PyClassRegistry Gex::Python::EvaluationProfiler_Wrap::registry;

bool Gex::Python::EvaluationNodeProfiler_Wrap::registered = false;
Gex::Python::PyClassRegistry Gex::Python::EvaluationNodeProfiler_Wrap::registry;





Gex::Python::Event_Wrap::Event_Wrap(): 
    Gex::Event(), pybind11::trampoline_self_life_support()
{

}


Gex::Python::Event_Wrap::Event_Wrap(const Gex::Event& base): 
    Gex::Event(base), pybind11::trampoline_self_life_support()
{

}


pybind11::object E_Duration(pybind11::args args,
                            pybind11::kwargs kwargs)
{
    const Gex::Event& evnt = args[0].cast<const Gex::Event&>();

    return pybind11::cast(
            evnt.Duration().count()
    );
}


pybind11::object E_StartTime(pybind11::args args,
                             pybind11::kwargs kwargs)
{
    const Gex::Event& evnt = args[0].cast<const Gex::Event&>();

    return pybind11::cast(
            evnt.StartTime().time_since_epoch()
    );
}


pybind11::object E_EndTime(pybind11::args args,
                                pybind11::kwargs kwargs)
{
    const Gex::Event& evnt = args[0].cast<const Gex::Event&>();

    return pybind11::cast(
            evnt.EndTime().time_since_epoch()
    );
}


bool Gex::Python::Event_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                    PyThreadState* state)
{
    if (registry.IsRegistered(state))
        return false;

    pybind11::class_<Event>(mod, "Event", pybind11::module_local(false))
            .def(pybind11::init<>())
            .def("Start", &Event::Start)
            .def("Stop", &Event::Stop)
            .def("Duration", E_Duration)
            .def("Running", &Event::Running)
            .def("StartTime", E_StartTime)
            .def("EndTime", E_EndTime)
            .def_readwrite("name", &Event::name)
            .def_readwrite("category", &Event::category)
            ;

    return registry.Register(state);
}


bool Gex::Python::Event_Wrap::IsRegistered()
{
    return registered;
}



Gex::Python::EvaluationProfiler_Wrap::EvaluationProfiler_Wrap():
    Gex::EvaluationProfiler()
{

}


Gex::Python::EvaluationProfiler_Wrap::EvaluationProfiler_Wrap(const Gex::EvaluationProfiler& base) :
    Gex::EvaluationProfiler(base)
{

}


pybind11::object EP_Result(
        pybind11::args args,
        pybind11::kwargs kwargs
)
{
    const Gex::EvaluationProfiler& self = args[0].cast<
            const Gex::EvaluationProfiler&>();

    pybind11::dict pyResult;
    for (const auto& pair : self.Result())
    {
        pybind11::list pyEvents;
        for (const auto& event : pair.second)
        {
            pyEvents.append(event);
        }

        pyResult[pybind11::cast(pair.first)] = pyEvents;
    }

    return pyResult;
}


pybind11::object EP_Duration(pybind11::args args,
                                  pybind11::kwargs kwargs)
{
    const Gex::EvaluationProfiler& p = args[0].cast<
            const Gex::EvaluationProfiler&>();

    return pybind11::cast(
            p.Duration().count()
    );
}


pybind11::object EP_StartTime(pybind11::args args,
                              pybind11::kwargs kwargs)
{
    const Gex::EvaluationProfiler& p = args[0].cast<
            const Gex::EvaluationProfiler&>();

    return pybind11::cast(p.StartTime().time_since_epoch());
}


pybind11::object EP_EndTime(pybind11::args args,
                                 pybind11::kwargs kwargs)
{
    const Gex::EvaluationProfiler& p = args[0].cast<
            const Gex::EvaluationProfiler&>();

    return pybind11::cast(
            p.EndTime().time_since_epoch()
    );
}


pybind11::object EP_StartEvent(pybind11::args args,
                                    pybind11::kwargs kwargs)
{
    Gex::EvaluationProfiler* self = args[0].cast<Gex::EvaluationProfiler*>();
    std::string category = args[1].cast<std::string>();
    std::string name = args[2].cast<std::string>();

    int idx = static_cast<int>(self->StartEvent(category, name));

    return pybind11::cast(idx);
}


pybind11::object EP_StopEvent(pybind11::args args,
                              pybind11::kwargs kwargs)
{
    Gex::EvaluationProfiler* self = args[0].cast<Gex::EvaluationProfiler*>();
    int idx = args[1].cast<int>();

    self->StopEvent(idx);

    return {};
}


bool Gex::Python::EvaluationProfiler_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                                 PyThreadState* state)
{
    if (registry.IsRegistered(state))
        return false;

    pybind11::class_<Gex::EvaluationProfiler, Gex::Python::EvaluationProfiler_Wrap, Profiler>(
            mod, "EvaluationProfiler", pybind11::module_local(false))
            .def("StartEvent", &EP_StartEvent)
            .def("StopEvent", &EP_StopEvent)
            .def("Start", &Gex::EvaluationProfiler::Start)
            .def("Stop", &Gex::EvaluationProfiler::Stop)
            .def("StartTime", &EP_StartTime)
            .def("EndTime", &EP_EndTime)
            .def("Duration", &EP_Duration)
            .def("Result", &EP_Result)
            ;

    mod.def("MakeProfiler", &Gex::MakeProfiler);

    return registry.Register(state);
}


bool Gex::Python::EvaluationProfiler_Wrap::IsRegistered()
{
    return registered;
}



Gex::Python::EvaluationNodeProfiler_Wrap::EvaluationNodeProfiler_Wrap(const Gex::EvaluationNodeProfiler& base) :
        Gex::EvaluationNodeProfiler(base), pybind11::trampoline_self_life_support()
{

}


pybind11::object ENP_Create(pybind11::args args,
                            pybind11::dict)
{
    Gex::EvaluationProfiler* profiler = args[0].cast<
            Gex::EvaluationProfiler*>();

    Gex::Node* node = args[1].cast<Gex::Node*>();

    std::string thread = args[2].cast<std::string>();

    return pybind11::cast(
            Gex::EvaluationNodeProfiler(
                    profiler->shared_from_this(),
                    node->shared_from_this(),
                    thread
            )
    );
}


bool Gex::Python::EvaluationNodeProfiler_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                                     PyThreadState* state)
{
    if (registry.IsRegistered(state))
        return false;

    pybind11::class_<Gex::Python::EvaluationNodeProfiler_Wrap>
            (mod, "NodeProfiler", pybind11::module_local(false))
            .def("StartEvent", &EvaluationNodeProfiler::StartEvent)
            .def("StopEvent", &EvaluationNodeProfiler::StopEvent)
            .def("GetProfiler", &EvaluationNodeProfiler::GetProfiler)
            .def_static("Create", &ENP_Create)
            ;

    return registry.Register(state);
}


bool Gex::Python::EvaluationNodeProfiler_Wrap::IsRegistered()
{
    return registered;
}
