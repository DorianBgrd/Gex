#include "ui/include/PluginLoader.h"

#include <windows.h>
#include <libloaderapi.h>

#include <filesystem>

#include "Gex/include/PluginLoader.h"


#define REGISTER_UI_PLUGIN_FUNC "RegisterUiPlugin"


typedef void(*RegisterUiPlugin)(Gex::Ui::UiPluginLoader*);
bool Gex::Ui::UiPluginLoader::initialized = false;


Gex::Ui::UiPluginLoader* Gex::Ui::UiPluginLoader::LoadPlugin(
        std::string path, Gex::Feedback* feedback)
{
    std::filesystem::path lpath = path;

    HINSTANCE module = LoadLibraryEx(lpath.wstring().c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!module)
    {
        if (feedback)
            feedback->Set(Gex::Status::Failed, "Could not load module " + path);
        return nullptr;
    }

    FARPROC func = GetProcAddress((HMODULE)module, REGISTER_UI_PLUGIN_FUNC);
    if (!func)
    {
        if (feedback)
            feedback->Set(Gex::Status::Failed, "Could not find function : " + path);
        return nullptr;
    }

    auto* loader = new UiPluginLoader();
    if (feedback)
        feedback->Set(Gex::Status::Success, "");

    loader->RegisterPluginPath(std::filesystem::absolute(path).string());

    auto function = (RegisterUiPlugin)func;
    function(loader);

    return loader;
}


void Gex::Ui::UiPluginLoader::LoadPluginCallback(std::string path)
{
    LoadPlugin(path);
}


Gex::Ui::UiPluginLoader::UiPluginLoader()
{
    engine = UiTSys::UiTypeEngine::GetEngine();
}


void Gex::Ui::UiPluginLoader::RegisterPluginPath(const std::string& path)
{
    pluginPath = path;
}


std::string Gex::Ui::UiPluginLoader::PluginPath() const
{
    return pluginPath;
}


bool Gex::Ui::UiPluginLoader::RegisterTypeWidget(
        size_t hash, const std::string& name,
        UiTSys::TypedInitWidgetCreator* initCreator,
        UiTSys::TypedWidgetCreator* creator, bool force)
{
    return engine->RegisterType(hash, name, initCreator, creator, force);
}


bool Gex::Ui::UiPluginLoader::Initialize()
{
    if (initialized)
        return false;

    Gex::PluginLoader::RegisterPluginCallback(
            UiPluginLoader::LoadPluginCallback);

    initialized = true;
    return initialized;
}
