#ifndef NODELIBRARY2COMPLETE_INTERPRETERKERNEL_H
#define NODELIBRARY2COMPLETE_INTERPRETERKERNEL_H

#include "softApi.h"
#include <string>
#include <functional>
#include <memory>

namespace SoftPython
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

        static void RegisterPythonWrapper();
    };

}

#endif //NODELIBRARY2COMPLETE_INTERPRETERKERNEL_H
