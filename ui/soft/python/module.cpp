#include "include/output.h"

#include "boost/python.hpp"




BOOST_PYTHON_MODULE(SoftPython)
{
    SoftPython::PythonOutput::RegisterPythonWrapper();
}
