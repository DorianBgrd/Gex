#include "ui/soft/include/MainWindow.h"


Gex::Editor::MainWindow::MainWindow(Gex::Graph* graph, QWidget* parent): QMainWindow(parent)
{
    auto* widget = new QWidget(this);
    setCentralWidget(widget);

    auto* layout = new QVBoxLayout();
    widget->setLayout(layout);

    graphWidget = new Gex::Ui::GraphWidget(graph, this);
    layout->addWidget(graphWidget);
}


void Gex::Editor::MainWindow::ShowMessage(Gex::Feedback feedback)
{
    auto uifeed = Gex::Ui::MakeFeedback(feedback);

    graphWidget->ShowMessage(uifeed);
}