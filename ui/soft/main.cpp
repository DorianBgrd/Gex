#include "include/Gex.h"
#include "ui/include/ui.h"
#include <QApplication>

#include <QObject>
#include <QWidget>
#include "ui/soft/include/MainWindow.h"

#include "ArgParse/ArgParse.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Parser parser;
    parser.AddFlag("-f", "--file", 1, "Graph file to open");

    parser.Parse(argc, argv);

    Gex::Graph* graph;

    Gex::Feedback feedback;
    if (parser.FlagFound("-f"))
    {
        graph = Gex::LoadGraph(parser.FlagResult("-f"), &feedback);
    }
    else
    {
        graph = new Gex::Graph();
        feedback.status = Gex::Status::Success;
        feedback.message = "Created new graph.";
    }

    Gex::Editor::MainWindow mainWindow(graph);

    mainWindow.show();

    mainWindow.ShowMessage(feedback);

    return app.exec();
}
