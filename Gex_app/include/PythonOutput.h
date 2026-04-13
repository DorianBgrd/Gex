#ifndef NODELIBRARY2COMPLETE_INTERPRETERKERNEL_H
#define NODELIBRARY2COMPLETE_INTERPRETERKERNEL_H

#include "softApi.h"
#include <string>
#include <functional>
#include <memory>

#include "pybind11/pybind11.h"

namespace Gex::App
{
    class PythonOutput;

    typedef std::shared_ptr<PythonOutput> PythonOutputPtr;

    class SOFT_API PythonOutput
    {
    private:
        static bool pythonRegistered;
        static PythonOutputPtr instance;

        std::vector<std::function<void(std::string)>> callbacks;
        PythonOutput() = default;

    public:
        static PythonOutputPtr GetInstance();

        void RegisterCallback(std::function<void(std::string)> callback);

        void writeMsg(std::string);

        static void write(std::string);

        static void RegisterPythonWrapper(pybind11::module_& mod);
    };

}

#endif //NODELIBRARY2COMPLETE_INTERPRETERKERNEL_H
