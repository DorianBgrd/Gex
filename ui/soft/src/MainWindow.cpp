#include "ui/soft/include/MainWindow.h"

#include <QMenuBar>
#include <QFileDialog>

#include "UiRes/include/uires.h"


Gex::Editor::MainWindow::MainWindow(Gex::Graph* graph, QWidget* parent): QMainWindow(parent)
{
    auto* widget = new QWidget(this);
    setCentralWidget(widget);

    auto* layout = new QVBoxLayout();
    widget->setLayout(layout);

    graphView = new Gex::Ui::GraphView(graph, this);
    layout->addWidget(graphView);

    auto* menu = new QMenuBar(this);
    auto* fileMenu =  menu->addMenu("File");
    fileMenu->addAction(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_folder_open),
                        "Open", this, &MainWindow::LoadPlugin);
    
    fileMenu->addAction(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_save),
                        "Save", this, &MainWindow::LoadPlugin);

    fileMenu->addAction(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_save),
                        "Save as", this, &MainWindow::LoadPlugin);

    auto* pluginMenu = menu->addMenu("Plugins");
    pluginMenu->addAction("Load plugin", this, &MainWindow::LoadPlugin);

    setMenuBar(menu);
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