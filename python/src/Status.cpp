#include "python/include/Status.h"
#include "boost/python.hpp"


bool Gex::Python::Feedback_Wrap::pythonRegistered = false;


bool Gex::Python::Feedback_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
        return false;

    boost::python::enum_<Gex::Status>("Status")
            .value(Gex::Status::None, "None")
            .value(Gex::Status::Success, "Success")
            .value(Gex::Status::Warning, "Warning")
            .value(Gex::Status::Failed, "Failed")
            .value(Gex::Status::Error, "Error");

    boost::python::class_<Gex::Feedback, boost::noncopyable>(
            "Feedback", boost::python::init())
            .attr("status")
            .attr("message")
            ;

    pythonRegistered = true;
    return pythonRegistered;
}