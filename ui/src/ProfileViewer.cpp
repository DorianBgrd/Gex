#include "ui/include/ProfileViewer.h"

#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>


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

    setBrush(QBrush(QColor("#15915B")));
}


Gex::Ui::ProfileWidget::ProfileWidget(QWidget* parent):
        BaseGraphView(new QGraphicsScene(), parent)
{
    setInteractive(true);
}


void Gex::Ui::ProfileWidget::ViewProfiler(ProfilerPtr p)
{
    profiler = p;

    size_t duration = p->Duration().count();

    setSceneRect(0, 0, duration, duration);
    fitInView(0, 0, duration, duration);

    size_t height = duration / profiler->NumberOfThreads();

    for (const auto& step : profiler->GetSteps())
    {
        auto* item = new ProfileStepItem(profiler, step);

        unsigned int incr = profiler->NodeThread(step->Node()->Path());

        size_t x = (step->StartTime() - profiler->StartTime()).count();
        size_t y = incr * height;
        size_t w = (step->EndTime() - step->StartTime()).count();
        size_t h = height;

        item->setRect(0, 0, w, h);
        item->setPos(x, y);
        scene()->addItem(item);
    }
}



Gex::Ui::ProfileView::ProfileView(QWidget* parent): QWidget(parent)
{
    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    profWidget = new ProfileWidget(this);
    layout->addWidget(profWidget);
}


void Gex::Ui::ProfileView::SetZoom(int zoom)
{
    double scale = (zoom / 100.0);

    profWidget->scale(scale, scale);
}


void Gex::Ui::ProfileView::FromContext(const Gex::GraphContext& context)
{
    profWidget->ViewProfiler(context.GetProfiler());
}


