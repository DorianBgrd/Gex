#ifndef GEX_PLUGINLOADER_H
#define GEX_PLUGINLOADER_H

#include "include/PluginLoader.h"


namespace Gex
{
    namespace Python
    {
        struct PluginLoader_Wrap
        {
        private:
            static bool pythonRegistered;
        public:
            static bool RegisterPythonWrapper();
        };
    }
}

#endif //GEX_PLUGINLOADER_H
