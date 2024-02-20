#include "ui/include/ProfileViewer.h"

#include <QGraphicsLineItem>
#include <QGraphicsRectItem>


inline double Duration(std::chrono::nanoseconds s)
{
    return std::chrono::duration<double>(s).count();
}


Gex::Ui::ProfileStepItem::ProfileStepItem(Gex::ProfilerPtr p, Gex::Step s, QGraphicsItem* parent): QGraphicsRectItem(parent)
{
    profiler = p;
    step = s;

    auto* textItem = new QGraphicsTextItem(this);
    textItem->setPlainText(step->Name().c_str());

    setBrush(QBrush(QColor("#FFFFFF")));
}


Gex::Ui::ProfileWidget::ProfileWidget(QWidget* parent): QGraphicsView(parent)
{
    auto* scene = new QGraphicsScene(this);
    setScene(scene);
}

void Gex::Ui::ProfileWidget::ViewProfiler(ProfilerPtr p)
{
    profiler = p;
    unsigned int thnb = profiler->NumberOfThreads();

    size_t duration = p->Duration().count();
    double ratio = rect().height() / rect().width();

    setSceneRect(0, 0, duration, ratio * duration);

    qreal h = sceneRect().height() / thnb;
    for (const auto& step : profiler->GetSteps())
    {
        auto* item = new ProfileStepItem(profiler, step);

        int incr = profiler->NodeThread(step->Node()->Path());
        size_t x = (step->StartTime() - profiler->StartTime()).count();
        size_t y = incr * h;
        size_t w = (step->EndTime() - step->StartTime()).count();

        item->setRect(0, 0, w, h);
        item->setPos(x, y);
        scene()->addItem(item);
    }
}


void Gex::Ui::ProfileWidget::FromContext(const Gex::GraphContext context)
{
    ViewProfiler(context.GetProfiler());
}