#ifndef GEX_SCHEDULEGRAPH_H
#define GEX_SCHEDULEGRAPH_H

#include "api.h"

#include <QPainter>
#include <QBrush>
#include <QWidget>
#include <QGraphicsObject>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QSplitter>
#include <QTreeWidget>

#include <vector>

#include "Gex/include/Gex.h"
#include "ui/include/BaseGraph.h"
#include "UiRes/include/uires.h"

namespace Gex::Ui
{
    class ScheduleLink;

    class SchedulePlugItem: public QGraphicsItem
    {
    public:
        using QGraphicsItem::QGraphicsItem;

        QRectF boundingRect() const override;

        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;
    };

    class ScheduleNodeItem: public QGraphicsObject
    {
        SchedulePlugItem* inputPlug;
        SchedulePlugItem* outputPlug;
        Gex::ScheduledNodeWkPtr node;
        QGraphicsTextItem* text;
        qreal width;

        QPainterPath path;

        QList<QSharedPointer<ScheduleLink>> links;

        qreal itemX = 0;

    public:
        ScheduleNodeItem(const Gex::ScheduledNodeWkPtr& scheduled,
                         QGraphicsItem* parent=nullptr);

        void SetX(qreal x);

        void InitPos();

        QRectF getBoundingRect() const;

        QRectF boundingRect() const override;

        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;

        QGraphicsItem* GetInput() const;

        QGraphicsItem* GetOutput() const;

        Gex::ScheduledNodeWkPtr GetScheduledNode() const;

        QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                            const QVariant &value) override;

        void RegisterLink(QSharedPointer<ScheduleLink>);

        void DeregisterLink(QSharedPointer<ScheduleLink>);
    };

    class ScheduleLink: public QGraphicsObject
    {
        QPen pen;
        QPainterPath path;

        ScheduleNodeItem* inputPtr;
        ScheduleNodeItem* outputPtr;

    public:
        ScheduleLink(ScheduleNodeItem* input,
                     ScheduleNodeItem* output,
                     QGraphicsItem* parent=nullptr);

        QRectF boundingRect() const override;

        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;

        QPainterPath shape() const override;
    };


    class ScheduleScene: public QGraphicsScene
    {
        Gex::ScheduleNodeWkPtrList nodes;
        QVector<ScheduleNodeItem*> items;

    public:
        explicit ScheduleScene(QObject* parent=nullptr);

        void Load(const Gex::ScheduleNodePtrList& nodes);
    };


    class ScheduleView: public BaseGraphView
    {
    public:
        ScheduleView(ScheduleScene* scene,
                     QWidget* parent=nullptr);
    };


    class ScheduleViewer: public QWidget
    {
        ScheduleView* view;
        ScheduleScene* scene;

        Gex::NodeWkPtr compound;

    public:
        ScheduleViewer(QWidget* parent=nullptr);

        void LoadCompound(const Gex::NodeWkPtr& compound);

        void Sync();
    };
}

#endif //GEX_SCHEDULEGRAPH_H
