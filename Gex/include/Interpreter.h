#ifndef GEX_INTERPRETER_H
#define GEX_INTERPRETER_H

#include "api.h"
#include "pybind11/pybind11.h"
#include "pybind11/embed.h"

#include "Wrappers.h"


namespace Gex::Python
{
    class Interpreter;

    class GEX_API InterpreterHandle
    {
        friend Interpreter;

        PyThreadState* mainState;
        PyThreadState* state = nullptr;
        uint64_t id = 0;

        bool initialized = false;
        PyInterpreterConfig* config = nullptr;

        InterpreterHandle(const InterpreterHandle& other) = delete;

    public:
        InterpreterHandle(PyThreadState* mainInterpreter,
                          PyInterpreterConfig* conf);

        ~InterpreterHandle();

        PyThreadState* GetThreadState() const;

        uint64_t GetInterpreterID() const;

        void Initialize();

        void Finalize();
    };


    typedef std::shared_ptr<InterpreterHandle> InterpreterHandlePtr;


    class GEX_API Interpreter
    {
        static bool initialized;

        static PyThreadState* mainState;
        static PyPreConfig* preConfig;
        static PyConfig* config;
        static PyInterpreterConfig* interpConfig;

    public:
        static PyPreConfig* GetPreConfig();

        static PyConfig* GetConfig();

        static PyInterpreterConfig* GetInterpConfig();

        static void SetPreConfig(PyPreConfig* config);

        static void SetConfig(PyConfig* config);

        static void SetInterpConfig(PyInterpreterConfig* interpConfig);

        static void Initialize();

        static InterpreterHandlePtr AcquireInterpreter();
    };
}

#endif //GEX_INTERPRETER_H
