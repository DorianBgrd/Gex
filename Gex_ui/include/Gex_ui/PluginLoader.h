#ifndef GEX_PLUGINLOADER_H
#define GEX_PLUGINLOADER_H

#include <string>

#include "Gex/include/Gex/Status.h"

#include "api.h"

#include "UiTsys/uitsys.h"


namespace Gex::Ui
{
    class ViewerRegistry;
    class NodeUiRegistry;
    class ViewerCreator;

    class GEX_UI_API UiPluginLoader
    {
        UiTSys::UiTypeEngine* engine;
        ViewerRegistry* registry;
        NodeUiRegistry* nodeUiRegistry = nullptr;

        std::string pluginPath;

        static bool initialized;

    public:
        static UiPluginLoader* LoadPlugin(std::string path, Gex::Feedback* result=nullptr);

        static void LoadPluginCallback(std::string path);

        UiPluginLoader();

        void RegisterPluginPath(const std::string& path);

        std::string PluginPath() const;

        bool RegisterType(const std::type_index& hash,
                          const std::string& uiName,
                          const QColor& color,
                          const UiTSys::TypedInitCreatorPtr& initCreator= nullptr,
                          const UiTSys::TypedCreatorPtr& creator= nullptr,
                          bool force= false);

        template<typename T>
        bool RegisterType(const std::string& uiName, const QColor& color,
                          const UiTSys::TypedInitCreatorPtr& initCreator=nullptr,
                          const UiTSys::TypedCreatorPtr& creator=nullptr,
                          bool force=false)
        {
            return RegisterType(typeid(T), uiName, color,
                                initCreator, creator, force);
        }

        template<typename T, typename InitCreatorType, typename WidgetCreatorType>
        bool RegisterType(const std::string& uiName, const QColor& color, bool force=false)
        {
            return RegisterType(
                    typeid(T), uiName, color,
                    MakeUiPtr(InitCreatorType),
                    MakeUiPtr(WidgetCreatorType),
                    force
            );
        }

        void RegisterNodeViewer(const std::string& type,
                                ViewerCreator* creator);

        template<class C>
        void RegisterNodeViewer(const std::string& type)
        {
            RegisterNodeViewer(type, new C());
        }

        void RegisterNodeResource(const std::string& type,
                                  const QIcon& icon,
                                  const QColor& color);

        QIcon GetNodeIcon(const std::string& type);

        static bool Initialize();
    };
}

#endif //GEX_PLUGINLOADER_H
