#ifndef GEX_GRAPHVIEW_H
#define GEX_GRAPHVIEW_H

#include <QWidget>
#include <QTabWidget>
#include <QDockWidget>
#include <QStackedLayout>

#include "NodeGraph.h"
#include "ProfileViewer.h"
#include "ScheduleGraph.h"

namespace Gex::Ui
{
    class GEX_UI_API ColoredFrame: public QWidget
    {
        QColor borderColor;
        QWidget* targetWidget = nullptr;

    public:
        ColoredFrame(QWidget* widget=nullptr);

        QColor Color() const;

        void SetColor(QColor color);

        QWidget* Target() const;

        void SetTarget(QWidget* target);

        void paintEvent(QPaintEvent *event) override;
    };


    class GEX_UI_API ColoredTabButton: public QPushButton
    {
    public:
        ColoredTabButton(const QString& text,
                         const QColor& color,
                         QWidget* parent=nullptr);


    };

    class GEX_UI_API TabWidget: public QWidget
    {
        Q_OBJECT

        QButtonGroup* group;

        ColoredFrame* coloredFrame;
        QStackedLayout* stackedLayout;
        QWidget* tabBar;
        QHBoxLayout* tabBarLayout;

    public:
        TabWidget(QWidget* parent=nullptr);

        void AddTab(const QString& title,
                    const QColor& color,
                    QWidget* widget);

//        QColor Color() const;
//
//        void SetColor(const QColor& color);
    };


    class GEX_UI_API GraphView: public QWidget
    {
        Q_OBJECT

    private:
        Gex::CompoundNodePtr graph;
        GraphWidget* graphView;
        ProfileView* profileView;
        ScheduleViewer* scheduleViewer;

    public:
        GraphView(Gex::CompoundNodePtr graph,
                  QWidget* parent=nullptr);

        GraphWidget* GetGraphWidget() const;

        ProfileView* GetProfileWidget() const;

        void SwitchGraph(Gex::CompoundNodePtr graph);
    };
}

#endif //GEX_GRAPHVIEW_H
