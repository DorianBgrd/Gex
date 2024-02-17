#ifndef GEX_MAINWINDOW_H
#define GEX_MAINWINDOW_H

#include <QMainWindow>
#include "ui/include/ui.h"


namespace Gex::Editor
{
    class MainWindow: public QMainWindow
    {
        Q_OBJECT

        Gex::Ui::GraphWidget* graphWidget;

    public:
        MainWindow(Gex::Graph* graph, QWidget* parent=nullptr);

        void ShowMessage(Gex::Feedback feedback);
    };
}

#endif //GEX_MAINWINDOW_H
