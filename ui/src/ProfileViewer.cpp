#include "ui/include/ProfileViewer.h"

#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>


inline double Duration(std::chrono::nanoseconds s)
{
    return std::chrono::duration<double>(s).count();
}


Gex::Ui::ProfileEventItem::ProfileEventItem(Gex::Profiler p, Gex::Event s,
                                            QGraphicsItem* parent):
                                            QGraphicsRectItem(parent)
{
    profiler = p;
    step = s;

    auto* textItem = new QGraphicsTextItem(this);
    textItem->setPlainText(step.name.c_str());

    setBrush(QBrush(QColor("#15915B")));
}


Gex::Ui::ProfileWidget::ProfileWidget(QWidget* parent):
        BaseGraphView(new QGraphicsScene(), parent)
{
    setInteractive(true);
}


void Gex::Ui::ProfileWidget::ViewProfiler(Gex::Profiler p)
{
    profiler = p;

    Gex::Profile prof = p->Result();

    size_t duration = p->Duration().count();

    setSceneRect(0, 0, duration, duration);

    size_t height = duration / prof.size();

    unsigned int y = 0;
    for (const auto& events : prof)
    {
        auto category = events.first;

        for (const auto& event : events.second)
        {
            auto* item = new ProfileEventItem(profiler, event);

            size_t x = (event.StartTime() - profiler->StartTime()).count();
            size_t w = event.Duration().count();
            size_t h = height;

            item->setRect(0, 0, w, h);
            item->setPos(x, y);
            scene()->addItem(item);
        }

        y += height;
    }

    fitInView(0, 0, duration, duration, Qt::IgnoreAspectRatio);

//    for (const auto& step : profiler->GetSteps())
//    {
//        auto* item = new ProfileStepItem(profiler, step);
//
//        unsigned int incr = profiler->NodeThread(step->Node()->Path());
//
//        size_t x = (step->StartTime() - profiler->StartTime()).count();
//        size_t y = incr * height;
//        size_t w = (step->EndTime() - step->StartTime()).count();
//        size_t h = height;
//
//        item->setRect(0, 0, w, h);
//        item->setPos(x, y);
//        scene()->addItem(item);
//    }
}



Gex::Ui::ProfileTable::ProfileTable(QWidget* parent): QTreeWidget(parent)
{
    headerItem()->setText(0, "Name");
    headerItem()->setText(1, "Duration (s)");
    headerItem()->setText(2, "Start (s)");
    headerItem()->setText(3, "End (s)");
}


QTreeWidgetItem* Gex::Ui::ProfileTable::CreateItem() const
{
    auto* item = new QTreeWidgetItem();
    item->setTextAlignment(0, Qt::AlignLeft);
    item->setTextAlignment(1, Qt::AlignRight);
    item->setTextAlignment(2, Qt::AlignRight);
    item->setTextAlignment(3, Qt::AlignRight);
    return item;
}


typedef double Seconds;

Seconds ToSec(Gex::Duration duration)
{
    return duration.count() / 10e9;
}


void Gex::Ui::ProfileTable::ViewProfiler(Gex::Profiler p)
{
    clear();

    Gex::Profile prof = p->Result();

    auto* graph = CreateItem();
    graph->setText(0, "Graph::Compute");
    graph->setText(1, std::to_string(ToSec(p->Duration())).c_str());
    graph->setText(2, "0");
    graph->setText(3, std::to_string(ToSec(p->EndTime() - p->StartTime())).c_str());
    addTopLevelItem(graph);

    for(auto events : prof)
    {
        auto* catItem = CreateItem();
        catItem->setText(0, events.first.c_str());
        addTopLevelItem(catItem);

        for (auto event : events.second)
        {
            auto* eventItem = CreateItem();
            catItem->addChild(eventItem);

            eventItem->setText(0, event.name.c_str());
            eventItem->setText(1, std::to_string(ToSec(event.Duration())).c_str());
            eventItem->setText(2, std::to_string(ToSec(event.StartTime() - p->StartTime())).c_str());
            eventItem->setText(3, std::to_string(ToSec(event.EndTime() - p->StartTime())).c_str());
        }
    }
}


Gex::Ui::ProfileView::ProfileView(QWidget* parent): QWidget(parent)
{
    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    auto* splitter = new QSplitter(this);
    layout->addWidget(splitter);

    profWidget = new ProfileWidget(this);
    splitter->addWidget(profWidget);

    profTable = new ProfileTable(this);
    splitter->addWidget(profTable);
}


void Gex::Ui::ProfileView::SetZoom(int zoom)
{
    double scale = (zoom / 100.0);

    profWidget->scale(scale, scale);
}


void Gex::Ui::ProfileView::FromContext(const Gex::Profiler& profiler)
{
    profWidget->ViewProfiler(profiler);
    profTable->ViewProfiler(profiler);
}


