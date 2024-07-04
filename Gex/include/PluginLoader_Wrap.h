#ifndef GEX_PLUGINLOADER_WRAP_H
#define GEX_PLUGINLOADER_WRAP_H

#include "api.h"
#include "PluginLoader.h"


namespace Gex
{
    namespace Python
    {
        struct GEX_API PluginLoader_Wrap
        {
        private:
            static bool pythonRegistered;
        public:
            static bool RegisterPythonWrapper();
        };
    }
}

#endif //GEX_PLUGINLOADER_WRAP_H
