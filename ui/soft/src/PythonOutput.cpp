#include "ui/soft/include/PythonOutput.h"

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


void SoftPython::PythonOutput::writeMsg(std::string write)
{
    for (const auto& cb : callbacks)
    {
        cb(write);
    }
}


void SoftPython::PythonOutput::write(std::string write)
{
    GetInstance()->writeMsg(write);
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
    boost::python::class_<SoftPython::PythonOutput>(
            "PythonOutput", boost::python::no_init)
            .def("write", &SoftPython::PythonOutput::write)
            .staticmethod("write")
            ;
}