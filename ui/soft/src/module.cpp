#include "include/PythonOutput.h"
#include "ui/soft/include/AppContext.h"

#include "boost/python.hpp"




BOOST_PYTHON_MODULE(SoftPython)
{
    SoftPython::PythonOutput::RegisterPythonWrapper();

    SoftApi::AppContext::RegisterPythonWrapper();

    SoftApi::SelectionContext::RegisterPythonWrapper();
}
