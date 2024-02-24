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

#include "Gex/include/Gex.h"
#include "ui/include/BaseGraph.h"

namespace Gex::Ui
{
    class GEX_UI_API ProfileEventItem: public QGraphicsRectItem
    {
        Gex::Event step;
        Gex::Profiler profiler;
    public:
        ProfileEventItem(Gex::Profiler profiler, Gex::Event step,
                         QGraphicsItem* parent=nullptr);
    };

    class GEX_UI_API ProfileWidget: public BaseGraphView
    {
        Q_OBJECT
    private:
        Gex::Profiler profiler;

    public:
        ProfileWidget(QWidget* parent=nullptr);

        void ViewProfiler(Gex::Profiler p);
    };


    class GEX_UI_API ProfileTable: public QTreeWidget
    {
        Q_OBJECT
    public:
        ProfileTable(QWidget* parent=nullptr);

        void ViewProfiler(Gex::Profiler p);

        QTreeWidgetItem* CreateItem() const;
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
