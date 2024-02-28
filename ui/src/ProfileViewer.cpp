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
    color = "#15915B";

    auto* textItem = new QGraphicsTextItem(this);
    textItem->setPlainText(step.name.c_str());

    setBrush(color);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
}


const Gex::Event& Gex::Ui::ProfileEventItem::GetEvent() const
{
    return step;
}


QVariant Gex::Ui::ProfileEventItem::itemChange(QGraphicsItem::GraphicsItemChange change,
                                               const QVariant &value)
{
    if (change == QGraphicsItem::GraphicsItemChange::ItemSelectedHasChanged)
    {
        if (value.toBool())
        {
            setBrush(color.lighter());
        }
        else
        {
            setBrush(color);
        }
    }

    return QGraphicsItem::itemChange(change, value);
}


Gex::Ui::ProfileWidget::ProfileWidget(QWidget* parent):
        BaseGraphView(new QGraphicsScene(), parent)
{
    setInteractive(true);

    QObject::connect(scene(), &QGraphicsScene::selectionChanged,
                     this, &ProfileWidget::OnSelection);
}


void Gex::Ui::ProfileWidget::ViewProfiler(Gex::Profiler p)
{
    profiler = p;

    scene()->clear();

    Gex::Profile prof = p->Result();

    size_t duration = p->Duration().count();

    setSceneRect(0, 0, duration * 2, duration);

    size_t height = duration / prof.size();

    unsigned int y = 0;
    for (const auto& events : prof)
    {
        auto category = events.first;

        for (const auto& event : events.second)
        {
            auto* item = new ProfileEventItem(profiler, event);

            size_t x = (event.StartTime() - profiler->StartTime()).count() * 2;
            size_t w = event.Duration().count() * 2;
            size_t h = height;

            item->setRect(0, 0, w, h);
            item->setPos(x, y);
            scene()->addItem(item);
        }

        y += height;
    }

    fitInView(0, 0, duration, duration, Qt::KeepAspectRatio);

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


void Gex::Ui::ProfileWidget::OnSelection()
{
    std::vector<Event> selected;
    for (auto elem : scene()->selectedItems())
    {
        auto* eventItem = dynamic_cast<ProfileEventItem*>(elem);
        if (!eventItem)
        {
            continue;
        }

        selected.push_back(eventItem->GetEvent());
    }

    Q_EMIT EventsSelected(selected);
}


void Gex::Ui::ProfileWidget::SelectEvents(std::vector<Event> events)
{
    blockSignals(true);
    for (auto* item : scene()->items())
    {
        item->setSelected(false);
    }

    for (auto* item : scene()->items())
    {
        auto* eventItem = dynamic_cast<ProfileEventItem*>(item);
        if (!eventItem)
            continue;

        if (std::find(events.begin(), events.end(), eventItem->GetEvent()) != events.end())
        {
            item->setSelected(true);
        }
        else
        {
            item->setSelected(false);
        }
    }
    blockSignals(false);
}


typedef double Seconds;

Seconds ToSec(Gex::Duration duration)
{
    return duration.count() / 10e9;
}


Gex::Ui::ProfileTreeItem::ProfileTreeItem(const Gex::Profiler& p,
                                          Event e, QTreeWidget* parent):
                                          QTreeWidgetItem(parent)
{
    event = e;
    setTextAlignment(0, Qt::AlignLeft);
    setTextAlignment(1, Qt::AlignRight);
    setTextAlignment(2, Qt::AlignRight);
    setTextAlignment(3, Qt::AlignRight);

    setText(0, event.name.c_str());
    setText(1, std::to_string(ToSec(event.Duration())).c_str());
    setText(2, std::to_string(ToSec(event.StartTime() - p->StartTime())).c_str());
    setText(3, std::to_string(ToSec(event.EndTime() - p->StartTime())).c_str());

}


Gex::Event Gex::Ui::ProfileTreeItem::GetEvent() const
{
    return event;
}



Gex::Ui::ProfileTable::ProfileTable(QWidget* parent): QTreeWidget(parent)
{
    headerItem()->setText(0, "Name");
    headerItem()->setText(1, "Duration (s)");
    headerItem()->setText(2, "Start (s)");
    headerItem()->setText(3, "End (s)");

    setSelectionMode(QTreeWidget::ExtendedSelection);

    QObject::connect(this, &QTreeWidget::itemSelectionChanged,
                     this, &ProfileTable::OnSelection);
}


void Gex::Ui::ProfileTable::OnSelection()
{
    std::vector<Event> selection;
    for (auto* item : selectedItems())
    {
        auto* eventItem = dynamic_cast<ProfileTreeItem*>(item);
        if (!eventItem)
        {
            continue;
        }

        selection.push_back(eventItem->GetEvent());
    }

    Q_EMIT EventsSelected(selection);
}


void Gex::Ui::ProfileTable::SelectEvents(std::vector<Event> events)
{
    blockSignals(true);

    clearSelection();

    int count = topLevelItemCount();
    for (unsigned int i = 0 ; i < count; i++)
    {
        auto* topItem = topLevelItem(i);
        for (unsigned int u = 0; u < topItem->childCount(); u++)
        {
            auto* child = topItem->child(u);
            auto* childEvent = dynamic_cast<ProfileTreeItem*>(child);
            if (!childEvent)
                continue;

            if (std::find(events.begin(), events.end(), childEvent->GetEvent()) != events.end())
            {
                childEvent->setSelected(true);
            }
        }
    }

    blockSignals(false);
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


void Gex::Ui::ProfileTable::keyReleaseEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_F)
    {
        auto selection = selectedItems();
        if (!selection.isEmpty())
        {
            scrollToItem(selection[0]);
        }
    }

    QTreeWidget::keyReleaseEvent(event);
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
            auto* eventItem = new ProfileTreeItem(p, event);
            catItem->addChild(eventItem);

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

    QObject::connect(profWidget, &ProfileWidget::EventsSelected,
                     profTable, &ProfileTable::SelectEvents);

    QObject::connect(profTable, &ProfileTable::EventsSelected,
                     profWidget, &ProfileWidget::SelectEvents);
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


