#include "Gex/include/PluginLoader.h"
#include <windows.h>
#include <libloaderapi.h>
#include <filesystem>

#include "boost/python.hpp"
#include "Gex/include/NodeFactory.h"

#include <fstream>


#define REGISTER_PLUGIN_FUNC_NAME "RegisterPlugin"


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


void Gex::PluginLoader::RegisterPluginPath(std::string path)
{
    pluginPath = std::move(path);
}


std::string Gex::PluginLoader::PluginPath()
{
    return pluginPath;
}


void Gex::PluginLoader::RegisterNode(std::string type, NodeBuilder* builder)
{
    std::string pluginName = std::filesystem::path(pluginPath).filename().string();
    builder->SetPlugin(pluginName);
    registeredBuilders[type] = builder;

    factory->RegisterNodeBuilder(type, builder);
}


std::vector<std::string> Gex::PluginLoader::AvailablePaths()
{
    return searchPaths;
}


typedef void (*RegisterFunction)(Gex::PluginLoader*);


std::filesystem::path Gex::PluginLoader::SearchFilePath(std::string name, Feedback* result)
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


bool Gex::PluginLoader::LoadPlugin(std::string name)
{
    bool success = false;

    LoadPlugin(name, success);

    return success;
}


Gex::PluginLoader* Gex::PluginLoader::LoadPlugin(std::string name, bool &result)
{
    auto* loader = new PluginLoader(NodeFactory::GetFactory(),
                                    TSys::TypeRegistry::GetRegistry());

    auto fileRes = new Feedback();
    std::filesystem::path libPath = SearchFilePath(name, fileRes);

    if (!fileRes)
    {
        result = false;
        return loader;
    }

    HINSTANCE module = LoadLibraryEx(libPath.string().c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!module)
    {
        result = false;
        return loader;
    }

    FARPROC func = GetProcAddress((HMODULE)module, REGISTER_PLUGIN_FUNC_NAME);
    if (!func)
    {
        result = false;
        return loader;
    }

    loader->RegisterPluginPath(std::filesystem::absolute(libPath.string()).string());
    auto function = (RegisterFunction)func;
    function(loader);
    result = true;

    for (auto cb : callbacks)
    {
        cb.second(libPath.string());
    }

    loadedPlugins.push_back(libPath.filename().string());

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


#define PLUGIN_LOAD_FAILED(MESSAGE) \
if (status)                         \
{                                   \
    status->status = Status::Failed; \
}                                   \
return loader;


//
//Gex::PluginLoader* Gex::PluginLoader::LoadPythonPlugin(std::string name, Feedback* status)
//{
//    auto *loader = new PluginLoader(NodeFactory::GetFactory());
//
//    Feedback fileRes;
//    std::filesystem::path libPath = SearchFilePath(name, &fileRes);
//
//    if (!fileRes) {
//        PLUGIN_LOAD_FAILED("Plugin not found.")
//    }
//
//    if (!Py_IsInitialized())
//    {
//        Py_InitializeEx(1);
//        PyObject* main = PyImport_AddModule("__main__");
//        if (!main)
//        {
//            PLUGIN_LOAD_FAILED("Could not initialize python.")
//        }
//    }
//
//    PyObject* sys = PyImport_ImportModule("sys");
//    PyObject* sysdict = PyModule_GetDict(sys);
//
//    PyObject* path = PyDict_GetItemString(sysdict, "path");
//    PyList_Insert(path, 0, PyUnicode_FromString(libPath.parent_path().string().c_str()));
//
//    std::string pluginName = libPath.stem().string();
//    PyObject* plugin = PyImport_ImportModule(pluginName.c_str());
//    if (!plugin)
//    {
//        PyErr_Print();
//        PLUGIN_LOAD_FAILED("Plugin \"" + pluginName + "\" could not be loaded.");
//    }
//
//    PyObject* pluginDict = PyModule_GetDict(plugin);
//
//    PyObject* func = PyDict_GetItem(pluginDict, PyUnicode_FromString(REGISTER_PLUGIN_FUNC_NAME));
//    if (func == Py_None)
//    {
//        PyErr_Print();
//        PLUGIN_LOAD_FAILED("Could not find \"" + std::string(REGISTER_PLUGIN_FUNC_NAME) + "\" function.");
//    }
//
//    Python_PluginLoader::RegisterPythonWrapper();
//
//    boost::python::object o = boost::python::object(Python_PluginLoader(loader));
////    bool res = boost::python::call<bool>(func, );
//
//    PyObject* res = PyObject_CallOneArg(func, o.ptr());
//    if (status && res)
//    {
//        status->status = Status::Success;
//    }
//
//    return loader;
//}
