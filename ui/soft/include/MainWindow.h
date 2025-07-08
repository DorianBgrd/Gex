#ifndef GEX_MAINWINDOW_H
#define GEX_MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include "ui/include/ui.h"

#include "PythonInterpreter.h"
#include "softApi.h"


namespace Gex::Editor
{
    class EventFilter: public QObject
    {
    public:
        EventFilter(QObject* p): QObject(p)
        {

        }

        bool eventFilter(QObject *watched, QEvent *event) override
        {
            if (event->type() == QEvent::FocusIn ||
                event->type() == QEvent::FocusAboutToChange ||
                event->type() == QEvent::FocusOut)
            {
                std::string typeName;
                if (event->type() == QEvent::FocusIn)
                    typeName = "FocusIn";
                else if (event->type() == QEvent::FocusAboutToChange)
                    typeName = "FocusAboutToChange";
                else
                    typeName = "FocusOut";

                std::cout << "Focus event " << typeName << std::endl;
                return true;
            }

            return false;
        }
    };

    class SOFT_API MainWindow: public QMainWindow
    {
        Q_OBJECT

        Gex::CompoundNodePtr graph;
        Gex::Ui::GraphView* graphView;
        std::string currentFile;
        QAction* saveAction;
        GexSoftware::PythonInterpreter* interpreter;
        Gex::Ui::ViewerDock* dock;
        Gex::Ui::TimelineDock* timeline;

    public:
        MainWindow(Gex::CompoundNodePtr graph, std::string file="",
                   QWidget* parent=nullptr);

        Gex::Feedback New();

        Gex::Feedback Open(const std::string& file);

        Gex::Feedback SaveAs(const std::string& file);

        Gex::Feedback Save();

        void NewCallback();

        void OpenCallback();

        void SaveAsCallback();

        void SaveCallback();

        void ShowMessage(const Gex::Feedback& feedback) const;

        void LoadPlugin();

        void Undo() const;

        void Redo() const;

        Gex::CompoundNodePtr CurrentNode() const;
    };
}

#endif //GEX_MAINWINDOW_H
