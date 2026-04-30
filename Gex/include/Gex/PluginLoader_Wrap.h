#ifndef GEX_PLUGINLOADER_WRAP_H
#define GEX_PLUGINLOADER_WRAP_H

#include "api.h"
#include "PluginLoader.h"

#include "Gex/python/utils.h"


namespace Gex
{
    namespace Python
    {
        struct GEX_API PluginLoader_Wrap
        {
        private:
            static bool pythonRegistered;
            static PyClassRegistry registry;

        public:
            static bool RegisterPythonWrapper(pybind11::module_& mod,
                                              PyThreadState* state=nullptr);

            static bool IsRegistered();
        };
    }
}

#endif //GEX_PLUGINLOADER_WRAP_H
