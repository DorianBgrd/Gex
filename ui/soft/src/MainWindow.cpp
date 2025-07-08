#include "ui/soft/include/MainWindow.h"

#include <QMenuBar>
#include <QFileDialog>
#include <QShortcut>

#include "UiRes/include/uires.h"

#include "Gex/include/Gex.h"


#include "ui/soft/include/AppContext.h"


Gex::Editor::MainWindow::MainWindow(Gex::CompoundNodePtr graph_, std::string file,
                                    QWidget* parent): QMainWindow(parent)
{
    installEventFilter(new EventFilter(this));

    auto* widget = new QWidget(this);
    setCentralWidget(widget);

    auto* layout = new QVBoxLayout();
    widget->setLayout(layout);

    graph = graph_;
    currentFile = file;

    // TODO : connect to node to update when node is added.
    graphView = new Gex::Ui::GraphView(graph, this);
    layout->addWidget(graphView);

    Gex::Ui::GraphWidget* graphWidget = graphView->GetGraphWidget();

    auto selCb = [](const Gex::NodeWkList& sel)
    {
        SoftApi::SelectionContext::Select(sel);
    };

    QObject::connect(graphWidget, &Gex::Ui::GraphWidget::SelectedNodeChanged, selCb);

    interpreter = new GexSoftware::PythonInterpreter(this);
    interpreter->Initialize();

    addDockWidget(Qt::RightDockWidgetArea, interpreter);

    dock = new Gex::Ui::ViewerDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    auto selectionGetter = [](){
        return SoftApi::SelectionContext::Selection();
    };

    dock->SetSelectionCallback(selectionGetter);

    // TODO : connect node to update when attribute is added.
    timeline = new Gex::Ui::TimelineDock(this);
    timeline->SetCurrentNode(graph);
    addDockWidget(Qt::BottomDockWidgetArea, timeline);

//    QObject::connect(graphView->GetGraphWidget(),
//                     &Gex::Ui::GraphWidget::SelectedNodeChanged,
//                     dock, &Gex::Ui::ViewerDock::NodeSelectionChanged);
    SoftApi::SelectionContext::Connect(
            [this, graphWidget](const Gex::NodeWkList& sel)
            {
                dock->NodeSelectionChanged(sel);
                graphWidget->Select(sel, false);
            }
    );

    auto* menu = new QMenuBar(this);
    menu->setFocusPolicy(Qt::NoFocus);
    menu->installEventFilter(new EventFilter(this));

    auto* fileMenu =  menu->addMenu("File");
    fileMenu->setFocusPolicy(Qt::NoFocus);

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

    auto* undo = new QShortcut(
            QKeySequence::fromString("Ctrl+Z"),
            this);

    QObject::connect(undo, &QShortcut::activated,
                     this, &MainWindow::Undo);

    auto* redo = new QShortcut(
            QKeySequence::fromString("Ctrl+Y"),
            this);

    QObject::connect(redo, &QShortcut::activated,
                     this, &MainWindow::Redo);

    SoftApi::AppContext::SetMainWindow(this);
}


Gex::Feedback Gex::Editor::MainWindow::New()
{
    Gex::Feedback feedback;

    currentFile = "";

    feedback.status = Gex::Status::Success;
    feedback.message = "New file";

    graph = Gex::CompoundNode::FromNode(
            Gex::NodeFactory::GetFactory()->CreateNode(
                    "CompoundNode", "Graph")
    );
    graph->SetName("Graph");
    graphView->SwitchGraph(graph);


    ShowMessage(feedback);

    Gex::Undo::UndoStack::Clear();

    return feedback;
}


Gex::Feedback Gex::Editor::MainWindow::Open(const std::string& file)
{
    Gex::Feedback feedback;
    Gex::NodePtr graph_ = Gex::LoadGraph(file, &feedback);

    currentFile = file;
    if ((feedback.status == Gex::Status::Failed) || (feedback.status == Gex::Status::Error))
    {
        graph_ = Gex::NodeFactory::GetFactory()->CreateNode(
                "CompoundNode", "Graph");
        currentFile = "";
    }

    graph = CompoundNode::FromNode(graph_);
    graphView->SwitchGraph(graph);

    Gex::Undo::UndoStack::Clear();

    ShowMessage(feedback);

    return feedback;
}

Gex::Feedback Gex::Editor::MainWindow::SaveAs(const std::string& file)
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



void Gex::Editor::MainWindow::ShowMessage(const Gex::Feedback& feedback) const
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


void Gex::Editor::MainWindow::Undo() const
{
    if (!Gex::Undo::UndoStack::Undo())
    {
        ShowMessage(Gex::Feedback::New(
                Gex::Status::Warning,
                "Undo stack is empty."));
    }
}


void Gex::Editor::MainWindow::Redo() const
{
    if (!Gex::Undo::UndoStack::Redo())
    {
        ShowMessage(Gex::Feedback::New(
                Gex::Status::Warning,
                "Redo stack is empty.")
        );
    }
}


Gex::CompoundNodePtr Gex::Editor::MainWindow::CurrentNode() const
{
    return graph;
}