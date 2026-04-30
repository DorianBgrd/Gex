#include "Gex/include/Gex/Interpreter.h"

#include "pybind11/pybind11.h"


PYBIND11_EMBEDDED_MODULE(Gex, mod, pybind11::multiple_interpreters::per_interpreter_gil())
{
    Gex::Python::RegisterPythonWrappers(mod, nullptr);
};


PyThreadState* Gex::Python::InterpreterHandle::GetThreadState() const
{
    return state;
}


uint64_t Gex::Python::InterpreterHandle::GetInterpreterID() const
{
    return id;
}


void Gex::Python::InterpreterHandle::Initialize()
{
    if (initialized)
        return;

#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 13
    PyInterpreterConfig interpConfig;
    interpConfig.gil = PyInterpreterConfig_OWN_GIL;

    Py_NewInterpreterFromConfig(&state, config);
    PyThreadState_Swap(state);

    id = PyInterpreterState_GetID(state->interp);
#endif

    initialized = true;
}


void Gex::Python::InterpreterHandle::Finalize()
{
    if (!initialized)
        return;

#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 13
    Py_EndInterpreter(state);

    PyThreadState_Swap(mainState);
#endif

    initialized = false;
}


Gex::Python::InterpreterHandle::InterpreterHandle(
        PyThreadState* main,
        PyInterpreterConfig* conf
)
{
    mainState = main;
    config = conf;
}


Gex::Python::InterpreterHandle::~InterpreterHandle()
{
    Finalize();
}


bool Gex::Python::Interpreter::initialized = false;
PyThreadState * Gex::Python::Interpreter::mainState = nullptr;
PyPreConfig* Gex::Python::Interpreter::preConfig = nullptr;
PyConfig* Gex::Python::Interpreter::config = nullptr;
PyInterpreterConfig* Gex::Python::Interpreter::interpConfig = nullptr;


PyPreConfig* Gex::Python::Interpreter::GetPreConfig()
{
    if (preConfig)
    {
        return preConfig;
    }

    auto* preconf = new PyPreConfig();
    PyPreConfig_InitPythonConfig(preconf);
    return preconf;
}


PyInterpreterConfig* Gex::Python::Interpreter::GetInterpConfig()
{
    if (interpConfig)
    {
        return interpConfig;
    }

#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 13
    auto* interpConf = new PyInterpreterConfig();
    interpConf->use_main_obmalloc = 0;
    interpConf->allow_fork = 0;
    interpConf->allow_exec = 0;
    interpConf->allow_threads = 1;
    interpConf->allow_daemon_threads = 0;
    interpConf->check_multi_interp_extensions = 1;
    interpConf->gil = PyInterpreterConfig_OWN_GIL;
#endif

    return interpConf;
}


void Gex::Python::Interpreter::SetPreConfig(PyPreConfig* preConf)
{
    preConfig = preConf;
}

void Gex::Python::Interpreter::SetConfig(PyConfig* conf)
{
    config = conf;
}

void Gex::Python::Interpreter::SetInterpConfig(PyInterpreterConfig* interpConf)
{
    interpConfig = interpConf;
}


PyConfig* Gex::Python::Interpreter::GetConfig()
{
    if (config)
    {
        return config;
    }

    auto* conf = new PyConfig();
    PyConfig_InitPythonConfig(conf);
    return conf;
}


void Gex::Python::Interpreter::Initialize()
{
    if (initialized)
        return;

    Py_PreInitialize(GetPreConfig());

    Py_InitializeFromConfig(GetConfig());

    mainState = PyThreadState_Get();

    initialized = true;
}


Gex::Python::InterpreterHandlePtr Gex::Python::Interpreter::AcquireInterpreter()
{
    auto handle = std::make_shared<InterpreterHandle>(mainState, GetInterpConfig());

    handle->Initialize();

    return handle;
}