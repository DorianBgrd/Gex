#include "Gex/include/References_Wrap.h"

#include "boost/python.hpp"


bool Gex::Python::References_Wrap::registered = false;


boost::python::object Python_ReferencesGetLoader(
        boost::python::tuple args,
        boost::python::dict kwargs)
{
    auto* refs = Gex::References::GetLoader();

    return boost::python::object(boost::python::ptr(refs));
}


boost::python::object Python_ReferencesAvailableRefs(
        boost::python::tuple args,
        boost::python::dict kwargs)
{
    boost::python::list dirs;

    Gex::References* refs = boost::python::extract<Gex::References*>(args[0]);

    for (auto d : refs->GetAvailableReferences())
    {
        dirs.append(d);
    }

    return dirs;
}


boost::python::object Python_ReferencesGetDirectories(
        boost::python::tuple args,
        boost::python::dict kwargs)
{
    boost::python::list dirs;

    Gex::References* refs = boost::python::extract<Gex::References*>(args[0]);

    for (auto d : refs->GetDirectories())
    {
        dirs.append(d);
    }

    return dirs;
}


bool Gex::Python::References_Wrap::RegisterPythonWrapper()
{
    if (registered)
        return false;

    boost::python::class_<Gex::References>("References", boost::python::no_init)
            .def("GetLoader", boost::python::raw_function(&Python_ReferencesGetLoader))
             .staticmethod("GetLoader")
             .def("AddDirectory", &Gex::References::AddDirectory)
             .def("RemDirectory", &Gex::References::RemDirectory)
             .def("GetAvailableReferences", boost::python::raw_function(
                     &Python_ReferencesAvailableRefs, 1))
             .def("GetDirectories", boost::python::raw_function(
                     &Python_ReferencesGetDirectories, 1))
             .def("ResolvePath", &Gex::References::ResolvePath)
             ;

    registered = true;
    return true;
}