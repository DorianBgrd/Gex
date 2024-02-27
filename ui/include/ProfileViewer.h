#ifndef GEX_PROFILEVIEWER_H
#define GEX_PROFILEVIEWER_H

#include "api.h"

#include <QPainter>
#include <QBrush>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QSplitter>
#include <QTreeWidget>

#include <vector>

#include "Gex/include/Gex.h"
#include "ui/include/BaseGraph.h"

namespace Gex::Ui
{
    class GEX_UI_API ProfileEventItem: public QGraphicsRectItem
    {
        Gex::Event step;
        Gex::Profiler profiler;
        QColor color;
    public:
        ProfileEventItem(Gex::Profiler profiler, Gex::Event step,
                         QGraphicsItem* parent=nullptr);

        const Event& GetEvent() const;

        QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                            const QVariant &value) override;
    };

    class GEX_UI_API ProfileWidget: public BaseGraphView
    {
        Q_OBJECT
    private:
        Gex::Profiler profiler;

    public:
        ProfileWidget(QWidget* parent=nullptr);

        void ViewProfiler(Gex::Profiler p);

    protected:
        void OnSelection();

    public:

        Q_SIGNAL void EventsSelected(std::vector<Event> event);

        void SelectEvents(std::vector<Event> event);
    };


    class GEX_UI_API ProfileTreeItem: public QTreeWidgetItem
    {
        Event event;
    public:
        ProfileTreeItem(const Gex::Profiler& p,
                        Event e, QTreeWidget* parent=nullptr);

        Event GetEvent() const;
    };


    class GEX_UI_API ProfileTable: public QTreeWidget
    {
        Q_OBJECT
    public:
        ProfileTable(QWidget* parent=nullptr);

        void ViewProfiler(Gex::Profiler p);

        QTreeWidgetItem* CreateItem() const;

    protected:
        void OnSelection();

    public:

        Q_SIGNAL void EventsSelected(std::vector<Event> event);

        void SelectEvents(std::vector<Event> event);
    };


    class GEX_UI_API ProfileView: public QWidget
    {
        Q_OBJECT
    private:
        Gex::Profiler profiler;
        ProfileWidget* profWidget;
        ProfileTable* profTable;

    public:
        ProfileView(QWidget* parent=nullptr);

        void FromContext(const Gex::Profiler& profiler);

    protected:
        void SetZoom(int zoom);
    };
}

#endif //GEX_PROFILEVIEWER_H
