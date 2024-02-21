#ifndef GEX_PROFILEVIEWER_H
#define GEX_PROFILEVIEWER_H

#include "api.h"

#include <QPainter>
#include <QBrush>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>

#include "include/Gex.h"
#include "ui/include/BaseGraph.h"

namespace Gex::Ui
{
    class GEX_UI_API ProfileStepItem: public QGraphicsRectItem
    {
        Gex::Step step;
        Gex::ProfilerPtr profiler;
    public:
        ProfileStepItem(Gex::ProfilerPtr profiler, Gex::Step step,
                        QGraphicsItem* parent=nullptr);
    };

    class GEX_UI_API ProfileWidget: public BaseGraphView
    {
        Q_OBJECT
    private:
        ProfilerPtr profiler;

    public:
        ProfileWidget(QWidget* parent=nullptr);

        void ViewProfiler(ProfilerPtr p);
    };


    class GEX_UI_API ProfileView: public QWidget
    {
        Q_OBJECT
    private:
        ProfilerPtr profiler;
        ProfileWidget* profWidget;

    public:
        ProfileView(QWidget* parent=nullptr);

        void FromContext(const Gex::GraphContext& context);

    protected:
        void SetZoom(int zoom);
    };
}

#endif //GEX_PROFILEVIEWER_H
