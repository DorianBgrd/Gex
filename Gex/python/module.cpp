#include "Gex/include/Wrappers.h"

#include "Tsys/tsys.h"

#include "boost/python.hpp"


boost::python::object GetAvailableTypes(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    boost::python::list types;
    for (const std::string& t : TSys::TypeRegistry::GetRegistry()->RegisteredTypes())
    {
        types.append(boost::python::object(t));
    }

    return types;
}


BOOST_PYTHON_MODULE(Gex_Python)
{

    boost::python::def(
            "GetAvailableTypes",
            boost::python::raw_function(
                    GetAvailableTypes
            )
    );


    Gex::Python::RegisterPythonWrappers();

}