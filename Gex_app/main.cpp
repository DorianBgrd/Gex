#include "Gex/include/Gex.h"
#include "Gex_ui/include/ui.h"
#include <QApplication>

#include <QObject>
#include <QWidget>
#include <QFile>
#include <QDir>
#include "Gex_ui/include/PluginLoader.h"
#include "Gex_app/include/MainWindow.h"

#include "Gex/include/Interpreter.h"

#include "ArgParse/ArgParse.h"

#include "pybind11/pybind11.h"

#include <windows.h>
#include "libloaderapi.h"

#include <pybind11/embed.h>

#include "Gex/include/Wrappers.h"

#include "Gex_app/include/AppContext.h"
#include "Gex_app/include/PythonOutput.h"


PYBIND11_EMBEDDED_MODULE(App, mod, pybind11::multiple_interpreters::per_interpreter_gil())
{
    Gex::App::AppContext::RegisterPythonWrapper(mod);

    Gex::App::SelectionContext::RegisterPythonWrapper(mod);

    Gex::App::PythonOutput::RegisterPythonWrapper(mod);
}


void InitializePython()
{
    auto* preConfig = new PyPreConfig();
    PyPreConfig_InitPythonConfig(preConfig);

//    Py_PreInitialize(&preConfig);

    auto* config = new PyConfig();
    PyConfig_InitPythonConfig(config);

    // ERROR : this should never be done !
    PyConfig_SetString(config, &config->home,
                       L"C:\\Program Files\\Python313");

    WCHAR p[MAX_PATH];
    GetModuleFileNameW(nullptr, p, MAX_PATH);

    std::filesystem::path path(p);

    std::wstring modulePath = path.parent_path().parent_path()
            .append("python").wstring();

    std::wstring currentEnv = L"";
    if (config->pythonpath_env)
        currentEnv = config->pythonpath_env;

    std::wstring newEnv = modulePath + L";" + currentEnv;

//    PyWideStringList_Append(&config.module_search_paths, modulePath.c_str());

//    config.module_search_paths_set = 1;
    config->pythonpath_env = newEnv.data();

    Gex::Python::Interpreter::SetPreConfig(preConfig);
    Gex::Python::Interpreter::SetConfig(config);

//    Py_InitializeFromConfig(&config);

//    PyConfig_Clear(&config);
//    Py_Initialize();

    Gex::Python::Interpreter::Initialize();

//    std::string code = "import sys\nsys.path.insert(0, r\"" + modulePath + "\")";

//    PyRun_SimpleString(code.c_str());
}


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Parser parser;
    parser.AddFlag("-f", "--file", 1, "Graph file to open");

    parser.Parse(argc, argv);

    InitializePython();

    Gex::CompoundNodePtr graph;

    Gex::Feedback feedback;
    if (parser.FlagFound("-f"))
    {
        auto g = Gex::LoadGraph(parser.FlagResult("-f"), &feedback);
        if (g)
            graph = Gex::CompoundNode::FromNode(g);
        else
            graph = Gex::CompoundNode::FromNode(
                    Gex::NodeFactory::GetFactory()->CreateNode("CompoundNode", "Graph")
            );
    }
    else
    {
        graph = Gex::CompoundNode::FromNode(
                Gex::NodeFactory::GetFactory()->CreateNode(
                        "CompoundNode", "Graph"
                )
        );
        feedback.status = Gex::Status::Success;
        feedback.message = "Created new graph.";
    }

    Gex::App::MainWindow mainWindow(graph);

    QFile styleFile("ui/stylesheet.css");
    styleFile.open(QFile::ReadOnly);
    QString style = styleFile.readAll();

    QString pluginsDirectory = "plugins";
    // Iter file directories.
    QDir pluginsDirs(pluginsDirectory);

    auto cb = [](std::string p)
            {Gex::Ui::UiPluginLoader::LoadPlugin(p);};

    Gex::PluginLoader::RegisterPluginCallback(cb);

    Gex::PluginLoader::AddSearchPath(pluginsDirs.absolutePath().toStdString());

    QStringList filters = {"*.json"};
    for (const auto& pluginDir : pluginsDirs.entryList(QDir::Dirs))
    {
        std::filesystem::path dirPath = pluginsDirs.filesystemAbsolutePath().append(
                pluginDir.toStdString());
        for (const auto& file : QDir(dirPath).entryList(filters))
        {
            Gex::Feedback feedback;

            bool success = Gex::PluginLoader::LoadPlugin(
                    dirPath.append(file.toStdString()).string(),
                    &feedback
            );

            if (!feedback)
            {
                Gex::LogFeedback(feedback);
            }
        }
    }

    app.setStyleSheet(style);

    mainWindow.show();

    mainWindow.ShowMessage(feedback);

    return app.exec();
}
