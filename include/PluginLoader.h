#ifndef NODELIBRARY2_PLUGINNODELOADER_H
#define NODELIBRARY2_PLUGINNODELOADER_H
#include <vector>
#include <string>
#include <filesystem>

#include "api.h"
#include "Status.h"
#include "NodeFactory.h"
#include "Tsys/include/tsys.h"


namespace Gex
{
    class GEX_API PluginLoader
    {
    protected:
        std::string pluginPath;
        std::map<std::string, NodeBuilder*> registeredBuilders;
        NodeFactory* factory;

        static std::vector<std::string> loadedPlugins;

    protected:
        static std::vector<std::string> searchPaths;

    public:
        static void AddSearchPath(std::string path);

    public:
        PluginLoader(NodeFactory* factory);

        [[nodiscard]]
        bool IsValid() const;

        [[nodiscard]]
        std::map<std::string, NodeBuilder*> RegisteredBuilders() const;

    protected:
        void RegisterPluginPath(std::string path);

    public:
        std::string PluginPath();

    public:
        template<class NodeBuilderClass>
        void RegisterNode(std::string type)
        {
            RegisterNode(type, new NodeBuilderClass());
        }

        void RegisterNode(std::string type, NodeBuilder* builder);

        template<class Type, class Handler>
        void RegisterTypeHandler()
        {
            TSys::TypeRegistry::GetRegistry()->RegisterType<Type, Handler>();
        }

    public:
        static std::vector<std::string> AvailablePaths();

        static std::filesystem::path SearchFilePath(std::string name, Feedback* result=nullptr);

        static bool LoadPlugin(std::string name);

        static PluginLoader* LoadPlugin(std::string name, bool &result);

        static bool PluginLoaded(std::string plugin);

//        static PluginLoader* LoadPythonPlugin(std::string name, Feedback* status=nullptr);
    };
}


#endif //NODELIBRARY2_PLUGINNODELOADER_H
