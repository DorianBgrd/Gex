#include "expression.h"

#include "Gex/include/NodeAttributeData_Wrap.h"

#include "boost/python/object.hpp"

Python::Expression::Expression(const std::string& expression)
{
    code = expression;
}

bool Python::Expression::Execute(Gex::NodeAttributeData& context)
{
    if (!Py_IsInitialized())
        Py_Initialize();

    auto* globals = PyDict_New();
    auto* pylocals = PyDict_New();

    Gex::Python::NodeAttributeData_Wrap::RegisterPythonWrapper();

    boost::python::object pyContext(context);

    PyDict_SetItem(pylocals, PyUnicode_FromString("context"), pyContext.ptr());

    PyObject* codeObject = Py_CompileString(code.c_str(), "<Node>",  Py_file_input);

    PyEval_EvalCode(codeObject, globals, pylocals);

    if(PyErr_Occurred())
    {
        PyErr_Print();
        return false;
    }

    return true;
}