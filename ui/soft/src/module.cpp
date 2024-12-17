#include "include/PythonOutput.h"

#include "boost/python.hpp"




BOOST_PYTHON_MODULE(SoftPython)
{
    SoftPython::PythonOutput::RegisterPythonWrapper();
}
