#include "Gex_app/include/PythonOutput.h"

#include "pybind11/pybind11.h"


bool Gex::App::PythonOutput::pythonRegistered  = false;
Gex::App::PythonOutputPtr Gex::App::PythonOutput::instance;


Gex::App::PythonOutputPtr Gex::App::PythonOutput::GetInstance()
{
    if (!instance)
        instance = std::make_shared<PythonOutput>(PythonOutput());

    return instance;
}


void Gex::App::PythonOutput::RegisterCallback(std::function<void(std::string)> callback)
{
    callbacks.push_back(callback);
}


void Gex::App::PythonOutput::writeMsg(std::string write)
{
    for (const auto& cb : callbacks)
    {
        cb(write);
    }
}


void Gex::App::PythonOutput::write(std::string write)
{
    GetInstance()->writeMsg(write);
}


pybind11::object PythonOutput_Write(pybind11::tuple args, pybind11::dict kwargs)
{
    Gex::App::PythonOutputPtr self = args[0].cast<Gex::App::PythonOutputPtr>();
    std::string buffer = args[1].cast<std::string>();

    self->write(buffer);
    return {};
}


void Gex::App::PythonOutput::RegisterPythonWrapper(pybind11::module_& mod)
{
    if (pythonRegistered)
        return;

    pybind11::class_<Gex::App::PythonOutput>(mod, "PythonOutput")
            .def_static("write", &Gex::App::PythonOutput::write)
            ;
    pythonRegistered = true;
}