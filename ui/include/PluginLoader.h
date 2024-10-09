#ifndef GEX_PLUGINLOADER_WRAP_H
#define GEX_PLUGINLOADER_H

#include <string>

#include "Gex/include/Status.h"

#include "ui/include/api.h"

#include "UiTsys/include/uitsys.h"


namespace Gex::Ui
{
    class ViewerRegistry;
    class ViewerCreator;

    class GEX_UI_API UiPluginLoader
    {
        UiTSys::UiTypeEngine* engine;
        ViewerRegistry* registry;
        std::string pluginPath;

        static bool initialized;

    public:
        static UiPluginLoader* LoadPlugin(std::string path, Gex::Feedback* result=nullptr);

        static void LoadPluginCallback(std::string path);

        UiPluginLoader();

        void RegisterPluginPath(const std::string& path);

        std::string PluginPath() const;

        bool RegisterTypeWidget(size_t hash, const std::string& uiName,
                                UiTSys::TypedInitWidgetCreator* initCreator=nullptr,
                                UiTSys::TypedWidgetCreator* creator=nullptr,
                                bool force=false);

        template<typename T>
        bool RegisterTypeWidget(const std::string& uiName,
                                UiTSys::TypedInitWidgetCreator* initCreator=nullptr,
                                UiTSys::TypedWidgetCreator* creator=nullptr,
                                bool force=false)
        {
            return RegisterTypeWidget(typeid(T).hash_code(), uiName,
                                      initCreator, creator, force);
        }

        template<typename T, typename InitCreatorType, typename WidgetCreatorType>
        bool RegisterTypeWidget(const std::string& uiName, bool force=false)
        {
            return RegisterTypeWidget(typeid(T).hash_code(),
                                      uiName, new InitCreatorType(),
                                      new WidgetCreatorType(), force);
        }

        void RegisterNodeViewer(const std::string& type,
                                ViewerCreator* creator);

        template<class C>
        void RegisterNodeViewer(const std::string& type)
        {
            RegisterNodeViewer(type, new C());
        }

        static bool Initialize();
    };
}

#endif //GEX_PLUGINLOADER_WRAP_H
