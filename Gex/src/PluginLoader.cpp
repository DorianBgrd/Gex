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
    AddSearchPath("");

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
    for (const std::string& p : pluginSearchPaths)
    {
        libPath = std::filesystem::path(p).append(name);

        if (!std::filesystem::exists(libPath))
        {
            continue;
        }

        if (result)
            result->Set(Status::Success, "");
        return libPath;
    }

    if (result)
    {
        result->Set(Status::Failed, "Failed to find plugin path.");
    }

    return libPath;
}


void Gex::PluginLoader::LoadCppPlugin(const std::string& name, PluginLoader* loader, Feedback* result)
{
    Feedback found;
    std::filesystem::path libPath = SearchFilePath(name, &found);

    if (!found)
    {
        if (result)
            result->Set(Status::Success, "Plugin file not found.");
        return;
    }

    std::filesystem::path absolutePath = std::filesystem::absolute(libPath.string());

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


void InitPython(Gex::Feedback* result=nullptr)
{
    if (!Py_IsInitialized())
    {
        Py_InitializeEx(1);
        PyObject* main = PyImport_AddModule("__main__");
        if (!main)
        {
            if (result)
                result->Set(Gex::Status::Failed, "Could not initialize python.");
            return;
        }

        Gex::Python::PluginLoader_Wrap::RegisterPythonWrapper();
    }
}


void Gex::PluginLoader::LoadPythonPlugin(const std::string& name, PluginLoader* loader, Feedback* result)
{

    InitPython();

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


Gex::PluginLoader* Gex::PluginLoader::LoadPluginFile(
        const std::string& name, const PluginType& type,
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

#ifdef GetObject
#undef GetObject
#endif


std::string ToAbsolutePath(const std::string& path,
                           const std::string& start)
{
    std::string st = path.substr(0, 2);

    std::filesystem::path p(path);
    if (st == "./" || st == ".\\")
    {
        std::filesystem::path relPath(
                path.substr(2, std::string::npos));

        p = std::filesystem::path(start) / relPath;
    }

    return p.string();
}


Gex::PluginLoader* Gex::PluginLoader::LoadPlugin(
        const std::string& pluginFile,
        Feedback* result)
{
    Feedback found;
    std::filesystem::path pluginPath = SearchFilePath(pluginFile, &found);
    if (!found)
    {
        if (result)
            result->Set(Status::Failed, "Plugin file not found.");
        return nullptr;
    }

    std::ifstream filestream(pluginPath);

    std::string content((std::istreambuf_iterator<char>(filestream)),
                        (std::istreambuf_iterator<char>()));

    filestream.close();

    rapidjson::Document json;
    json.Parse(content.c_str());

    if (!json.IsObject())
    {
        if (result)
            result->Set(Status::Failed, "Plugin file content was not readable.");
        return nullptr;
    }

    rapidjson::Value& dict = json.GetObject();

    auto conf = Config::GetConfig();
    if (!dict.HasMember(conf.pluginConfPlugKey.c_str()))
    {
        if (result)
            result->Set(Status::Failed, "Missing " + conf.pluginConfPlugKey);
        return nullptr;
    }

    std::string start = pluginPath.parent_path().string();

    std::string pluginPilePath = ToAbsolutePath(dict[conf.pluginConfPlugKey.c_str()].GetString(), start);

    if(!dict.HasMember(conf.pluginConfTypeKey.c_str()))
    {
        if (result)
            result->Set(Status::Failed, "Missing " + conf.pluginConfTypeKey);
        return nullptr;
    }

    auto pluginType = PluginType(dict[conf.pluginConfTypeKey.c_str()].GetInt());

    if (dict.HasMember(conf.pluginConfEnvKey.c_str()))
    {
        rapidjson::Value& envDict = dict[conf.pluginConfEnvKey.c_str()];
        for (auto iter = envDict.MemberBegin(); iter != envDict.MemberEnd(); iter++)
        {
            std::string env = iter->name.GetString();

            std::string value;
            for (unsigned int i = 0; i < iter->value.Size(); i++)
            {
                if (i > 0)
                    value += ";";

                // Path could be relative, make it absolute.
                value += ToAbsolutePath(iter->value[i].GetString(), start);
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

    if (dict.HasMember(conf.pluginConfPyEnvKey.c_str()))
    {
        InitPython();

        PyObject* osModule = PyImport_ImportModule("os");
        PyObject* osEnviron = PyDict_GetItem(PyModule_GetDict(osModule),
                                             PyUnicode_FromString("environ"));

        rapidjson::Value& pyEnvDict = dict[conf.pluginConfPyEnvKey.c_str()];
        for (auto iter = pyEnvDict.MemberBegin(); iter != pyEnvDict.MemberEnd(); iter++)
        {
            const char* env = iter->name.GetString();

            std::string value;
            for (unsigned int i = 0; i < iter->value.Size(); i++)
            {
                if (i > 0)
                    value += ";";

                value += ToAbsolutePath(iter->value[i].GetString(), start);
            }

            PyObject* pykey = PyUnicode_FromString(env);
            PyObject* pyenv = PyDict_GetItem(osEnviron, pykey);
            if (pyenv)
            {
                PyUnicode_Append(&pyenv, PyUnicode_FromString(";"));
                PyUnicode_Append(&pyenv, PyUnicode_FromString(value.c_str()));
            }
            else
            {
                pyenv = PyUnicode_FromString(value.c_str());
            }

            PyDict_SetItem(osEnviron, pykey, pyenv);
        }
    }

    if (dict.HasMember(conf.pluginConfPyEnvKey.c_str()))
    {
        InitPython();

        PyObject* sysModule = PyImport_ImportModule("sys");
        PyObject* sysPath = PyDict_GetItem(PyModule_GetDict(sysModule),
                                           PyUnicode_FromString("path"));

        rapidjson::Value& pyPathList = dict[conf.pluginConfPyEnvKey.c_str()];
        for (unsigned int sp = 0; sp < pyPathList.Size(); sp++)
        {
            PyList_Append(sysPath, PyUnicode_FromString(ToAbsolutePath(
                    pyPathList[sp].GetString(), start).c_str()));
        }
    }

    return LoadPluginFile(pluginPilePath, pluginType, result);
}


bool Gex::PluginLoader::PluginLoaded(std::string plugin)
{
    return (std::find(loadedPlugins.begin(), loadedPlugins.end(), plugin) != loadedPlugins.end());
}


std::vector<std::string> Gex::PluginLoader::LoadedPlugins()
{
    return loadedPlugins;
}

