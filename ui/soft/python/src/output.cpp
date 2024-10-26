#include "ui/soft/python/include/output.h"

#include "boost/python.hpp"


bool SoftPython::PythonOutput::pythonRegistered  = false;
SoftPython::PythonOutputPtr SoftPython::PythonOutput::instance;


SoftPython::PythonOutputPtr SoftPython::PythonOutput::GetInstance()
{
    if (!instance)
        instance.reset(new PythonOutput());

    return instance;
}


void SoftPython::PythonOutput::RegisterCallback(std::function<void(std::string)> callback)
{
    callbacks.push_back(callback);
}


void SoftPython::PythonOutput::write(std::string write)
{
    for (auto cb : callbacks)
    {
        cb(write);
    }
}


boost::python::object PythonOutput_Write(boost::python::tuple args, boost::python::dict kwargs)
{
    SoftPython::PythonOutputPtr self = boost::python::extract<SoftPython::PythonOutputPtr>(args[0]);
    std::string buffer = boost::python::extract<std::string>(args[1]);

    self->write(buffer);
    return {};
}


void SoftPython::PythonOutput::RegisterPythonWrapper()
{
    boost::python::class_<SoftPython::PythonOutput, SoftPython::PythonOutputPtr>(
            "PythonOutput", boost::python::no_init)
            .def("write", boost::python::raw_function(&PythonOutput_Write, 1))
            .def("GetInstance", &SoftPython::PythonOutput::GetInstance)
            .staticmethod("GetInstance")
            ;
}