#ifndef NODELIBRARY2_PLUGINNODELOADER_H
#define NODELIBRARY2_PLUGINNODELOADER_H
#include <vector>
#include <string>
#include <filesystem>

#include "defs.h"
#include "api.h"
#include "Status.h"
#include "NodeFactory.h"
#include "Tsys/include/tsys.h"


#define PLUGIN_PATHS_ENV "GEX_PLUGIN_PATHS"


namespace Gex
{
    enum class PluginType
    {
        Library,
        Python
    };

    class GEX_API PluginLoader
    {
    private:
        static bool initialized;

        static void Initialize();
    protected:
        std::string pluginPath;
        std::map<std::string, NodeBuilder*> registeredBuilders;
        NodeFactory* factory;
        TSys::TypeRegistry* registry;

        static std::vector<std::string> loadedPlugins;

        static CallbackId nextIndex;

    protected:
        static std::vector<std::string> searchPaths;
        static std::map<CallbackId, PluginLoadedCallback> callbacks;

    public:
        static void AddSearchPath(std::string path);

        static CallbackId RegisterPluginCallback(PluginLoadedCallback callback);

        static bool DeregisterPluginCallback(CallbackId id);

    public:
        PluginLoader(NodeFactory* factory, TSys::TypeRegistry* registry);

        [[nodiscard]]
        bool IsValid() const;

        [[nodiscard]]
        std::map<std::string, NodeBuilder*> RegisteredBuilders() const;

    protected:
        void RegisterPluginPath(const std::string& path);

    public:
        std::string PluginPath();

    public:
        template<class NodeBuilderClass>
        void RegisterNode(std::string type)
        {
            RegisterNode(type, new NodeBuilderClass());
        }

        void RegisterNode(const std::string& type, NodeBuilder* builder);

        template<class Type, class Handler>
        void RegisterTypeHandler()
        {
            registry->RegisterType<Type, Handler>();
        }

    public:
        static std::vector<std::string> AvailablePaths();

        static std::filesystem::path SearchFilePath(const std::string& name, Feedback* result=nullptr);

    protected:
        static void LoadCppPlugin(const std::string& name,
                                  PluginLoader* loader,
                                  Feedback* result=nullptr);

        static void LoadPythonPlugin(const std::string& name,
                                     PluginLoader* loader,
                                     Feedback* result=nullptr);

        static PluginLoader* LoadPluginFile(const std::string& name,
                                            const PluginType& type,
                                            Feedback* result=nullptr);

    public:
        static PluginLoader* LoadPlugin(const std::string& name,
                                        Feedback* result=nullptr);

        static bool PluginLoaded(std::string plugin);

        static std::vector<std::string> LoadedPlugins();

        static void LoadConfigFile(std::string envFile);
    };
}


#endif //NODELIBRARY2_PLUGINNODELOADER_H
