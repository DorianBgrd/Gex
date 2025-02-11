#include "Gex/include/Profiler_Wrap.h"


bool Gex::Python::Event_Wrap::registered = false;
bool Gex::Python::EvaluationProfiler_Wrap::registered = false;
bool Gex::Python::EvaluationNodeProfiler_Wrap::registered = false;




Gex::Python::Event_Wrap::Event_Wrap(): Gex::Event(), boost::python::wrapper<Gex::Event>()
{

}


Gex::Python::Event_Wrap::Event_Wrap(const Gex::Event& base) : Gex::Event(base), boost::python::wrapper<Event>()
{

}


boost::python::object E_Duration(boost::python::tuple args,
                                 boost::python::dict kwargs)
{
    const Gex::Event& evnt = boost::python::extract<
            const Gex::Event&>(args[0]);

    return boost::python::object(
            evnt.Duration().count()
    );
}


boost::python::object E_StartTime(boost::python::tuple args,
                                  boost::python::dict kwargs)
{
    const Gex::Event& evnt = boost::python::extract<
            const Gex::Event&>(args[0]);

    return boost::python::object(
            evnt.StartTime().time_since_epoch()
    );
}


boost::python::object E_EndTime(boost::python::tuple args,
                                boost::python::dict kwargs)
{
    const Gex::Event& evnt = boost::python::extract<
            const Gex::Event&>(args[0]);

    return boost::python::object(
            evnt.EndTime().time_since_epoch()
    );
}


void Gex::Python::Event_Wrap::RegisterPythonWrapper()
{
    if (registered)
        return;

    boost::python::class_<Event>("Event", boost::python::init())
            .def("Start", &Event::Start)
            .def("Stop", &Event::Stop)
            .def("Duration", boost::python::raw_function(E_Duration))
            .def("Running", &Event::Running)
            .def("StartTime", boost::python::raw_function(E_StartTime))
            .def("EndTime", boost::python::raw_function(E_EndTime))
            .def_readwrite("name", &Event::name)
            .def_readwrite("category", &Event::category)
            ;

    registered = true;
}



Gex::Python::EvaluationProfiler_Wrap::EvaluationProfiler_Wrap():
    Gex::EvaluationProfiler(),
    boost::python::wrapper<Gex::EvaluationProfiler>()
{

}


Gex::Python::EvaluationProfiler_Wrap::EvaluationProfiler_Wrap(const Gex::EvaluationProfiler& base) :
    Gex::EvaluationProfiler(base), boost::python::wrapper<EvaluationProfiler>()
{

}


boost::python::object EP_Result(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    const Gex::EvaluationProfiler& self = boost::python::extract<
            const Gex::EvaluationProfiler&>(args[0]);

    boost::python::dict pyResult;
    for (const auto& pair : self.Result())
    {
        boost::python::list pyEvents;
        for (const auto& event : pair.second)
        {
            pyEvents.append(event);
        }

        pyResult[pair.first] = pyEvents;
    }

    return pyResult;
}


boost::python::object EP_Duration(boost::python::tuple args,
                                  boost::python::dict kwargs)
{
    const Gex::EvaluationProfiler& p = boost::python::extract<
            const Gex::EvaluationProfiler&>(args[0]);

    return boost::python::object(
            p.Duration().count()
    );
}


boost::python::object EP_StartTime(boost::python::tuple args,
                                   boost::python::dict kwargs)
{
    const Gex::EvaluationProfiler& p = boost::python::extract<
            const Gex::EvaluationProfiler&>(args[0]);

    return boost::python::object(p.StartTime().time_since_epoch());
}


boost::python::object EP_EndTime(boost::python::tuple args,
                                 boost::python::dict kwargs)
{
    const Gex::EvaluationProfiler& p = boost::python::extract<
            const Gex::EvaluationProfiler&>(args[0]);

    return boost::python::object(
            p.EndTime().time_since_epoch()
    );
}


boost::python::object EP_StartEvent(boost::python::tuple args,
                                    boost::python::dict kwargs)
{
    Gex::EvaluationProfiler* self = boost::python::extract<Gex::EvaluationProfiler*>(args[0]);
    std::string category = boost::python::extract<std::string>(args[1]);
    std::string name = boost::python::extract<std::string>(args[2]);

    int idx = static_cast<int>(self->StartEvent(category, name));

    return boost::python::object(idx);
}


boost::python::object EP_StopEvent(boost::python::tuple args,
                                   boost::python::dict kwargs)
{
    Gex::EvaluationProfiler* self = boost::python::extract<Gex::EvaluationProfiler*>(args[0]);
    int idx = boost::python::extract<int>(args[1]);

    self->StopEvent(idx);

    return {};
}


void Gex::Python::EvaluationProfiler_Wrap::RegisterPythonWrapper()
{
    if (registered)
        return;

    boost::python::class_<Gex::Python::EvaluationProfiler_Wrap, Gex::Profiler>("EvaluationProfiler", boost::python::no_init)
            .def("StartEvent", boost::python::raw_function(&EP_StartEvent))
            .def("StopEvent", boost::python::raw_function(&EP_StopEvent))
            .def("Start", &Gex::EvaluationProfiler::Start)
            .def("Stop", &Gex::EvaluationProfiler::Stop)
            .def("StartTime", boost::python::raw_function(EP_StartTime))
            .def("EndTime", boost::python::raw_function(EP_EndTime))
            .def("Duration", boost::python::raw_function(EP_Duration))
            .def("Result", boost::python::raw_function(EP_Result))
            ;

    boost::python::def("MakeProfiler", &Gex::MakeProfiler);

    registered = true;
}



Gex::Python::EvaluationNodeProfiler_Wrap::EvaluationNodeProfiler_Wrap(const Gex::EvaluationNodeProfiler& base) :
        Gex::EvaluationNodeProfiler(base), boost::python::wrapper<EvaluationNodeProfiler>()
{

}


boost::python::object ENP_Create(boost::python::tuple args,
                                 boost::python::dict)
{
    Gex::EvaluationProfiler* profiler = boost::python::extract<
            Gex::EvaluationProfiler*>(args[0]);

    Gex::Node* node = boost::python::extract<Gex::Node*>(args[1]);

    std::string thread = boost::python::extract<std::string>(args[2]);

    return boost::python::object(
            Gex::EvaluationNodeProfiler(
                    profiler->shared_from_this(),
                    node->shared_from_this(),
                    thread
            )
    );
}


void Gex::Python::EvaluationNodeProfiler_Wrap::RegisterPythonWrapper()
{
    if (registered)
        return;

    boost::python::class_<Gex::Python::EvaluationNodeProfiler_Wrap>
            ("NodeProfiler", boost::python::no_init)
            .def("StartEvent", &EvaluationNodeProfiler::StartEvent)
            .def("StopEvent", &EvaluationNodeProfiler::StopEvent)
            .def("GetProfiler", &EvaluationNodeProfiler::GetProfiler)
            .def("Create", boost::python::raw_function(&ENP_Create))
            .staticmethod("Create")
            ;

    registered = true;
}
