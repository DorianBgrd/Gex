#include "ui/soft/include/MainWindow.h"

#include <QMenuBar>
#include <QFileDialog>

#include "UiRes/include/uires.h"


Gex::Editor::MainWindow::MainWindow(Gex::Graph* graph_, std::string file,
                                    QWidget* parent): QMainWindow(parent)
{
    auto* widget = new QWidget(this);
    setCentralWidget(widget);

    auto* layout = new QVBoxLayout();
    widget->setLayout(layout);

    graph = graph_;
    currentFile = file;

    graphView = new Gex::Ui::GraphView(graph, this);
    layout->addWidget(graphView);

    auto* menu = new QMenuBar(this);
    auto* fileMenu =  menu->addMenu("File");
//    fileMenu->addAction(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_folder_open),
//                        "Open", this, &MainWindow::New);
    fileMenu->addAction(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_file),
                        "New", this, &MainWindow::NewCallback);

    fileMenu->addAction(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_folder_open),
                        "Open", this, &MainWindow::OpenCallback);

    fileMenu->addAction(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_save),
                        "Save as", this, &MainWindow::SaveAsCallback);

    saveAction = fileMenu->addAction(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_save),
                        "Save", this, &MainWindow::SaveCallback);

    saveAction->setEnabled(!currentFile.empty());

    auto* pluginMenu = menu->addMenu("Plugins");
    pluginMenu->addAction("Load plugin", this, &MainWindow::LoadPlugin);

    setMenuBar(menu);
}


Gex::Feedback Gex::Editor::MainWindow::New()
{
    Gex::Feedback feedback;

    currentFile = "";

    feedback.status = Gex::Status::Success;
    feedback.message = "New file";

    if (graph)
    {
        delete graph;
    }

    graph = new Gex::Graph();
    graphView->SwitchGraph(graph);

    ShowMessage(feedback);

    return feedback;
}


Gex::Feedback Gex::Editor::MainWindow::Open(std::string file)
{
    Gex::Feedback feedback;
    Gex::Graph* graph_ = Gex::LoadGraph(file, &feedback);

    currentFile = file;
    if ((feedback.status == Gex::Status::Failed) || (feedback.status == Gex::Status::Error))
    {
        graph_ = new Gex::Graph();
        currentFile = "";
    }

    if (graph)
    {
        delete graph;
    }

    graph = graph_;
    graphView->SwitchGraph(graph);

    ShowMessage(feedback);

    return feedback;
}

Gex::Feedback Gex::Editor::MainWindow::SaveAs(std::string file)
{
    Gex::Feedback f;
    if (!std::filesystem::exists(file))
    {
        auto dir = std::filesystem::path(file).parent_path();
        if (!std::filesystem::exists(dir))
        {
            f.message = "Directory " + dir.string() + " does not exist.";
            f.status = Gex::Status::Failed;
            return f;
        }
    }

    currentFile = file;
    return Gex::SaveGraph(graph, file);
}


Gex::Feedback Gex::Editor::MainWindow::Save()
{
    if (currentFile.empty())
    {
        Gex::Feedback f;
        f.message = "No current file set on editor.";
        f.status = Gex::Status::Error;
        return f;
    }

    return SaveAs(currentFile);
}


void Gex::Editor::MainWindow::NewCallback()
{
    New();
}


void Gex::Editor::MainWindow::OpenCallback()
{
    QString file = QFileDialog::getOpenFileName(this, "Open", "", "*.json");

    if (file.isEmpty())
    {
        return;
    }

    auto r = Open(file.toStdString());

    saveAction->setEnabled(!currentFile.empty());
}


void Gex::Editor::MainWindow::SaveAsCallback()
{
    QString file = QFileDialog::getSaveFileName(this, "Save", "", "*.json");

    if (file.isEmpty())
    {
        return;
    }

    SaveAs(file.toStdString());
}


void Gex::Editor::MainWindow::SaveCallback()
{
    Save();
}



void Gex::Editor::MainWindow::ShowMessage(Gex::Feedback feedback)
{
    auto uifeed = Gex::Ui::MakeFeedback(feedback);

    graphView->GetGraphWidget()->ShowMessage(uifeed);
}


void Gex::Editor::MainWindow::LoadPlugin()
{
    std::string dir;
    if (!PluginLoader::AvailablePaths().empty())
    {
        dir = PluginLoader::AvailablePaths()[0];
    }

    QFileDialog dialog(this, "Load plugin", dir.c_str(), "*.dll");

    if (dialog.exec())
    {
        QStringList sel = dialog.selectedFiles();

        if (sel.isEmpty())
        {
            return;
        }

        for (const auto& f : sel)
        {
            PluginLoader::LoadPlugin(f.toStdString());
        }
    }
}