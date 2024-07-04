#include "Gex/include/Status_Wrap.h"
#include "Gex/include/Status.h"
#include "boost/python.hpp"


bool Gex::Python::Feedback_Wrap::pythonRegistered = false;


boost::python::object StatusGet(boost::python::tuple args,
                                boost::python::dict kwargs)
{
    Gex::Feedback* feedback = boost::python::extract<Gex::Feedback*>(args[0]);
    return boost::python::object(feedback->status);
}


boost::python::object StatusSet(boost::python::tuple args,
                                boost::python::dict kwargs)
{
    Gex::Feedback* feedback = boost::python::extract<Gex::Feedback*>(args[0]);
    Gex::Status st = boost::python::extract<Gex::Status>(args[1]);

    feedback->status = st;

    return {};
}


boost::python::object MessageGet(boost::python::tuple args,
                                boost::python::dict kwargs)
{
    Gex::Feedback* feedback = boost::python::extract<Gex::Feedback*>(args[0]);
    return boost::python::object(feedback->message);
}


boost::python::object MessageSet(boost::python::tuple args,
                                boost::python::dict kwargs)
{
    Gex::Feedback* feedback = boost::python::extract<Gex::Feedback*>(args[0]);
    std::string st = boost::python::extract<std::string>(args[1]);

    feedback->message = st;

    return {};
}


bool Gex::Python::Feedback_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
        return false;

    boost::python::enum_<Gex::Status>("Status")
            .value("None", Gex::Status::None)
            .value("Success", Gex::Status::Success)
            .value("Warning", Gex::Status::Warning)
            .value("Failed", Gex::Status::Failed)
            .value("Error", Gex::Status::Error)
            ;


    boost::python::class_<Gex::Feedback>(
            "Feedback", boost::python::init())
            .add_property("status", boost::python::raw_function(&StatusGet),
                          boost::python::raw_function(&StatusSet))
            .add_property("message", boost::python::raw_function(&MessageGet),
                          boost::python::raw_function(&MessageSet))
            ;

    pythonRegistered = true;
    return pythonRegistered;
}