#include "Gex/include/Gex.h"
#include "ui/include/ui.h"
#include <QApplication>

#include <QObject>
#include <QWidget>
#include <QFile>
#include <QDir>
#include "ui/include/PluginLoader.h"
#include "ui/soft/include/MainWindow.h"

#include "ArgParse/ArgParse.h"

#include "Python.h"

#include <windows.h>
#include "libloaderapi.h"


void InitializePython()
{
//    PyConfig config;
//    PyConfig_InitPythonConfig(&config);
//
//    WCHAR p[MAX_PATH];
//    GetModuleFileNameW(nullptr, p, MAX_PATH);
//
//    std::filesystem::path path(p);
//    PyWideStringList_Append(&config.module_search_paths, path.parent_path().parent_path()
//        .append("python").wstring().c_str());
//
//    Py_InitializeFromConfig(&config);

    Py_Initialize();

    WCHAR p[MAX_PATH];
    GetModuleFileNameW(nullptr, p, MAX_PATH);

    std::filesystem::path path(p);

    std::string modulePath = path.parent_path().parent_path()
            .append("python").string();

    std::string code = "import sys\nsys.path.insert(0, r\"" + modulePath + "\")";

    PyRun_SimpleString(code.c_str());
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
                Gex::NodeFactory::GetFactory()->CreateNode("CompoundNode", "Graph")
                );
        feedback.status = Gex::Status::Success;
        feedback.message = "Created new graph.";
    }

    Gex::Editor::MainWindow mainWindow(graph);

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
            Gex::PluginLoader::LoadPlugin(dirPath.append(file.toStdString()).string());
        }
    }

    app.setStyleSheet(style);

    mainWindow.show();

    mainWindow.ShowMessage(feedback);

    return app.exec();
}
