#ifndef GEX_MAINWINDOW_H
#define GEX_MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include "ui/include/ui.h"


namespace Gex::Editor
{
    class MainWindow: public QMainWindow
    {
        Q_OBJECT

        Gex::Graph* graph;
        Gex::Ui::GraphView* graphView;
        std::string currentFile;
        QAction* saveAction;

    public:
        MainWindow(Gex::Graph* graph, std::string file="",
                   QWidget* parent=nullptr);

        Gex::Feedback New();

        Gex::Feedback Open(std::string file);

        Gex::Feedback SaveAs(std::string file);

        Gex::Feedback Save();

        void NewCallback();

        void OpenCallback();

        void SaveAsCallback();

        void SaveCallback();

        void ShowMessage(Gex::Feedback feedback);

        void LoadPlugin();
    };
}

#endif //GEX_MAINWINDOW_H
