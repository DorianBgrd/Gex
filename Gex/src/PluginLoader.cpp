#include "Gex/include/PluginLoader.h"
#include "rapidjson/document.h"
#include "Gex/include/Config.h"

#include <windows.h>
#include <libloaderapi.h>
#include <filesystem>

#include "boost/python.hpp"
#include "Gex/include/NodeFactory.h"

#include "Gex/include/PluginLoader_Wrap.h"

#include <fstream>
#include <cstdlib>
#include <stdlib.h>


#define REGISTER_PLUGIN_FUNC_NAME "RegisterPlugin"


bool Gex::PluginLoader::initialized = false;
std::map<Gex::CallbackId, Gex::PluginLoadedCallback> Gex::PluginLoader::callbacks;
std::vector<std::string> Gex::PluginLoader::searchPaths;
std::vector<std::string> Gex::PluginLoader::loadedPlugins;

Gex::CallbackId Gex::PluginLoader::nextIndex = 0;


Gex::PluginLoader::PluginLoader(NodeFactory* f,
                                TSys::TypeRegistry* r)
{
    factory = f;
    registry = r;
}


void Gex::PluginLoader::AddSearchPath(std::string path)
{
    searchPaths.push_back(path);
}


Gex::CallbackId Gex::PluginLoader::RegisterPluginCallback(PluginLoadedCallback callback)
{
    nextIndex++;
    callbacks[nextIndex] = callback;
    return nextIndex;
}


bool Gex::PluginLoader::DeregisterPluginCallback(CallbackId id)
{
    auto iter = callbacks.find(id);
    if (iter == callbacks.end())
        return false;

    callbacks.erase(iter);
    return true;
}


bool Gex::PluginLoader::IsValid() const
{
    return (!registeredBuilders.empty());
}


std::map<std::string, Gex::NodeBuilder*> Gex::PluginLoader::RegisteredBuilders() const
{
    return registeredBuilders;
}


void Gex::PluginLoader::RegisterPluginPath(const std::string& path)
{
    pluginPath = path;
}


std::string Gex::PluginLoader::PluginPath()
{
    return pluginPath;
}


void Gex::PluginLoader::RegisterNode(const std::string& type, NodeBuilder* builder)
{
    std::string pluginName = std::filesystem::path(pluginPath).filename().string();
    builder->SetPlugin(pluginName);
    registeredBuilders[type] = builder;

    factory->RegisterNodeBuilder(type, builder);
}


#ifdef WIN32
#define SEP ';'
#else
#define SEP ':'
#endif


void Gex::PluginLoader::Initialize()
{
    if (!initialized)
    {
        char* strenv = std::getenv(PLUGIN_PATHS_ENV);
        if (strenv)
        {
            std::string env = strenv;

            auto count = std::count(env.begin(), env.end(), SEP);

            for (size_t i = 0; i <= count; i++)
            {
                auto nextSep = env.find(SEP);
                AddSearchPath(env.substr(0, nextSep));

                env = env.substr(nextSep + 1, std::string::npos);
            }

        }

        initialized = true;
    }
}


std::vector<std::string> Gex::PluginLoader::AvailablePaths()
{
    Initialize();

    return searchPaths;
}


typedef void (*RegisterFunction)(Gex::PluginLoader*);


std::filesystem::path Gex::PluginLoader::SearchFilePath(const std::string& name, Feedback* result)
{
    std::vector<std::string> pluginSearchPaths = AvailablePaths();

    // Add current path and place to be able to load absolute paths
    // plugins, like in DeserializeFromFile().
    pluginSearchPaths.emplace_back(std::filesystem::current_path().string());
    pluginSearchPaths.emplace_back("");

    std::filesystem::path libPath;
    bool found = false;

    for (const std::string& p : pluginSearchPaths)
    {
        libPath = std::filesystem::path(p);
        libPath.append(name);

        if (!std::filesystem::exists(libPath))
        {
            continue;
        }

        found = true;
        break;
    }

    if (!found)
    {
        if (result)
        {
            result->status = Status::Failed;
        }
    }

    return libPath;
}


bool Gex::PluginLoader::LoadPlugin(const std::string& name, const PluginType& type)
{
    Feedback success;

    LoadPlugin(name, type, &success);

    return success;
}


void Gex::PluginLoader::LoadCppPlugin(const std::string& name, PluginLoader* loader, Feedback* result)
{
    std::filesystem::path libPath = SearchFilePath(name, result);

    if (!result)
    {
        if (result)
            result->Set(Status::Success, "Plugin file not found.");
        return;
    }

    std::filesystem::path absolutePath = std::filesystem::absolute(libPath.string());
    std::filesystem::path envFile = absolutePath.replace_extension(".json");
    if (std::filesystem::exists(envFile))
    {
        LoadConfigFile(envFile.string());
    }

    HINSTANCE module = LoadLibraryEx(libPath.string().c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!module)
    {
        if (result)
            result->Set(Status::Failed, "Could not load module");
        return;
    }

    FARPROC func = GetProcAddress((HMODULE)module, REGISTER_PLUGIN_FUNC_NAME);
    if (!func)
    {
        if (result)
            result->Set(Status::Failed, "Could not find" +
                                        std::string(REGISTER_PLUGIN_FUNC_NAME) +
                                        " function.");
        return;
    }

    loader->RegisterPluginPath(absolutePath.string());
    auto function = (RegisterFunction)func;
    function(loader);

    if (result)
        result->Set(Status::Success, "");

    loadedPlugins.push_back(libPath.filename().string());

    for (auto cb : callbacks)
    {
        cb.second(libPath.string());
    }
}


void Gex::PluginLoader::LoadPythonPlugin(const std::string& name, PluginLoader* loader, Feedback* result)
{

    if (!Py_IsInitialized())
    {
        Py_InitializeEx(1);
        PyObject* main = PyImport_AddModule("__main__");
        if (!main)
        {
            if (result)
                result->Set(Status::Failed, "Could not initialize python.");
            return;
        }

        Python::PluginLoader_Wrap::RegisterPythonWrapper();
    }

    // Load environment file if found.
    PyObject* pkgutil = PyImport_ImportModule("pkgutil");
    if (pkgutil)
    {
        PyObject* pkgutilDict = PyModule_GetDict(pkgutil);
        PyObject* getLoaderFunc = PyDict_GetItem(pkgutilDict, PyUnicode_FromString("get_loader"));

        PyObject* loaderArgs = PyTuple_New(1);
        PyTuple_SetItem(loaderArgs, 0, PyUnicode_FromString(name.c_str()));
        PyObject* loaderKwargs = PyDict_New();

        PyObject* moduleLoader = PyObject_Call(getLoaderFunc, loaderArgs, loaderKwargs);

        PyObject* filename = PyObject_CallMethodOneArg(
                moduleLoader, PyUnicode_FromString("get_filename"),
                PyUnicode_FromString(name.c_str()));

        std::string moduleFile  = PyUnicode_AsUTF8(filename);

        std::filesystem::path modulePath = moduleFile;
        if (modulePath.filename() == "__init__.py")
        {
            modulePath = modulePath.parent_path();
        }

        std::filesystem::path directory = modulePath.parent_path();
        std::filesystem::path envFile = directory.append(name + ".json");

        if (std::filesystem::exists(envFile))
        {
            LoadConfigFile(envFile.string());
        }
    }

    PyObject* mod = PyImport_ImportModule(name.c_str());
    if (!mod)
    {
        PyErr_Print();
        if (result)
            result->Set(Status::Failed, "Failed loading plugin.");
        return;
    }

    PyObject* moddict = PyModule_GetDict(mod);

    PyObject* func = PyDict_GetItem(moddict, PyUnicode_FromString(REGISTER_PLUGIN_FUNC_NAME));
    if (func == Py_None)
    {
        PyErr_Print();
        if (result)
            result->Set(Status::Failed, "Could not find function " +
                std::string(REGISTER_PLUGIN_FUNC_NAME) + " in plugin module.");
        return;
    }

    boost::python::object o = boost::python::object(
            boost::python::ptr(loader));

    PyObject* res = PyObject_CallOneArg(func, o.ptr());
    if (!res)
    {
        if (result)
            result->Set(Status::Failed, "Failed calling " +
                std::string(REGISTER_PLUGIN_FUNC_NAME));
    }

    if (result)
        result->Set(Status::Success, "");

    for (const auto& cb : callbacks)
    {
        cb.second(name);
    }
}


Gex::PluginLoader* Gex::PluginLoader::LoadPlugin(const std::string& name, const PluginType& type,
                                                 Feedback* result)
{
    auto* loader = new PluginLoader(NodeFactory::GetFactory(),
                                    TSys::TypeRegistry::GetRegistry());

    switch (type)
    {
        case PluginType::Library:
            LoadCppPlugin(name, loader, result);
            break;

        case PluginType::Python:
            LoadPythonPlugin(name, loader, result);
            break;
    }

    return loader;
}


bool Gex::PluginLoader::PluginLoaded(std::string plugin)
{
    return (std::find(loadedPlugins.begin(), loadedPlugins.end(), plugin) != loadedPlugins.end());
}


std::vector<std::string> Gex::PluginLoader::LoadedPlugins()
{
    return loadedPlugins;
}


#ifdef GetObject
#undef GetObject
#endif


void Gex::PluginLoader::LoadConfigFile(std::string envFile)
{
    std::ifstream filestream(envFile);

    std::string content((std::istreambuf_iterator<char>(filestream)),
                        (std::istreambuf_iterator<char>()));

    filestream.close();

    rapidjson::Document json;
    json.Parse(content.c_str());

    rapidjson::Value& dict = json.GetObject();

    auto conf = Config::GetConfig();
    if (dict.HasMember(conf.pluginConfEnvKey.c_str()))
    {
        auto& envDict = dict[conf.pluginConfEnvKey.c_str()];

        for (auto iter = envDict.MemberBegin(); iter != envDict.MemberEnd(); iter++)
        {
            std::string env = iter->name.GetString();

            std::string value;
            for (unsigned int i = 0; i < iter->value.Size(); i++)
            {
                if (i > 0)
                    value += ";";
                value += iter->value[i].GetString();
            }

            const char* current = std::getenv(env.c_str());
            if (current)
            {
                value = std::string(current) + ";" + value;
            }

            if (!SetEnvironmentVariableA(env.c_str(), value.c_str()))
            {
                std::cerr << "# Warning : Failed to set env var " << env << "." << std::endl;
            }
        }
    }
}

