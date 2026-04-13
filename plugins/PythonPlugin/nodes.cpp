#include "nodes.h"
#include "Tsys/defaultTypes.h"

#include "Gex/include/Gex.h"
#include "Gex/include/Graph_Wrap.h"
#include "Gex/include/Wrappers.h"

#include "Gex/include/interpreter.h"


void GexPython::Nodes::Expression::InitAttributes()
{
    CreateAttribute<std::string>(
            "code",
            Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<std::string>(
            "exception",
            Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}


std::string GetExcTraceback(PyObject* pyExc)
{
    std::string formatted = "";

    if (PyObject* traceback = PyException_GetTraceback(pyExc))
    {
        if (PyObject* tracebackModule = PyImport_ImportModule("traceback"))
        {
            if (PyObject* formatException = PyObject_GetAttrString(
                    tracebackModule, "format_exception"
            ))
            {
                PyObject *args = PyTuple_Pack(1, pyExc);

                if (PyObject *result = PyObject_CallObject(formatException, args))
                {
                    PyObject* join = PyUnicode_FromString("");
                    PyObject* formattedStr = PyUnicode_Join(join, result);

                    formatted = PyUnicode_AsUTF8(formattedStr);

                    Py_DECREF(join);
                    Py_DECREF(formattedStr);
                    Py_DECREF(result);
                }

                Py_DECREF(formatException);
            }

            Py_DECREF(tracebackModule);
        }

        Py_DECREF(traceback);
    }

    return formatted;
}


void StoreInputs(PyObject* inputs, std::vector<Gex::AttributeWkPtr> attrs)
{
    for (const auto& attr : attrs)
    {
        if (!attr->IsInput())
        {
            continue;
        }

        std::string name = attr->Name();

        PyDict_SetItem(
                inputs, PyUnicode_FromString(name.c_str()),
                attr->TypeHandle()->ToPython(attr->GetAnyValue()).ptr()
       );
    }
}


void SaveOutputs(PyObject* outputs, Gex::NodeWkPtr node,
                 Gex::NodeAttributeData& data)
{
    PyObject* vars = PyDict_Keys(outputs);
    PyObject* values = PyDict_Values(outputs);
    for (unsigned int i = 0; i < PyList_Size(vars); i++)
    {
        PyObject* var = PyList_GetItem(vars, i);
        if (!PyUnicode_Check(var))
        {
            continue;
        }

        std::string variableName = PyUnicode_AsUTF8(var);

        if (auto attr = node->GetAttribute(variableName))
        {
            data.GetAttribute(variableName).SetValue(
                    pybind11::reinterpret_borrow<pybind11::object>(
                            PyList_GetItem(values, i)
                    )
            );
        }
    }

}


bool GexPython::Nodes::Expression::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    if (!Py_IsInitialized())
    {
        Py_Initialize();
    }

//    Python::Expression expr(
//
//    );

    auto code = context.GetAttribute("code").GetValue<std::string>();

    Gex::Python::Interpreter::Initialize();

    auto interp = Gex::Python::Interpreter::AcquireInterpreter();

    auto* globals = PyDict_New();
    auto* pylocals = PyDict_New();

    StoreInputs(pylocals, GetAllAttributes());

    PyObject* codeObject = Py_CompileString(code.c_str(), "<Node>",  Py_file_input);

    PyEval_EvalCode(codeObject, globals, pylocals);

    std::string traceback;

    bool success = true;
    if(PyErr_Occurred())
    {
        auto* exc = PyErr_GetRaisedException();

        if (exc)
        {
            traceback = GetExcTraceback(exc);
            success = false;
        }

        Py_DECREF(exc);
    }

    SaveOutputs(pylocals, shared_from_this(), context);

    context.GetAttribute("exception").SetValue(traceback);

    return success;
}



void GexPython::Nodes::InlineExpression::InitAttributes()
{
    CreateAttribute<std::string>(
            "expression",
            Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<TSys::AnyValue>("Inputs", Gex::AttrValueType::Multi,
                                    Gex::AttrType::Input);

    CreateAttribute<TSys::AnyValue>("Output", Gex::AttrValueType::Single,
                                    Gex::AttrType::Output);

    CreateAttribute<std::string>(
            "exception",
            Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}


bool GexPython::Nodes::InlineExpression::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    if (!Py_IsInitialized())
    {
        Py_Initialize();
    }

//    Python::Expression expr(
//
//    );

    auto expr = context.GetAttribute("expression").GetValue<std::string>();

    Gex::Python::Interpreter::Initialize();

    auto interp = Gex::Python::Interpreter::AcquireInterpreter();

    auto* globals = PyDict_New();
    auto* pylocals = PyDict_New();

    auto inputContext = context.GetAttribute("Inputs");
    auto indices = inputContext.GetIndices();

    unsigned int max = 0;
    if (!indices.empty())
        max = *(std::max_element(indices.begin(), indices.end()));

    PyObject* pythonValues = PyList_New(max);

    PyList_New(max);

    for (unsigned int index : indices)
    {
        pybind11::object val = inputContext
                .GetIndexValue<TSys::AnyValue>(index)
                .Python_Get();

        PyList_SetItem(pythonValues, index, val.ptr());
    }

    PyDict_SetItem(pylocals, PyUnicode_FromString("Inputs"), pythonValues);

    PyObject* codeObject = Py_CompileString(expr.c_str(), "<Node>",  Py_eval_input);

    bool success = true;
    std::string traceback;

    PyObject* output = nullptr;

    if(PyErr_Occurred())
    {
        auto* exc = PyErr_GetRaisedException();

        if (exc)
        {
            traceback = GetExcTraceback(exc);
            success = false;
        }

        Py_DECREF(exc);
    }
    else
    {
        output = PyEval_EvalCode(codeObject, globals, pylocals);


        if(PyErr_Occurred())
        {
            auto* exc = PyErr_GetRaisedException();

            if (exc)
            {
                traceback = GetExcTraceback(exc);
                success = false;
            }

            Py_DECREF(exc);
        }
    }


    TSys::AnyValue out;
    if (output)
        out.Python_Set(pybind11::reinterpret_steal<pybind11::object>(output));
    context.GetAttribute("Output").SetValue(out);

    context.GetAttribute("exception").SetValue(traceback);

    return success;
}
