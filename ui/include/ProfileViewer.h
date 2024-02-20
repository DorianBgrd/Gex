#ifndef GEX_PROFILEVIEWER_H
#define GEX_PROFILEVIEWER_H

#include "api.h"

#include <QPainter>
#include <QBrush>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>

#include "include/Gex.h"

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

    class GEX_UI_API ProfileWidget: public QGraphicsView
    {
        Q_OBJECT
    private:
        ProfilerPtr profiler;

    public:
        ProfileWidget(QWidget* parent=nullptr);

        void ViewProfiler(ProfilerPtr p);

        void FromContext(Gex::GraphContext context);
    };
}

#endif //GEX_PROFILEVIEWER_H
