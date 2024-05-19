#ifndef GEX_UI_BEZIERFUNC_H
#define GEX_UI_BEZIERFUNC_H

#include "plugins/InputRel/BezierFunc.h"

#include "plugins/export.h"

#include "UiTsys/include/uitsys.h"


#include <QPainterPath>

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>


namespace Gex::InputRel
{
    class BezierPointItem;
    class BezierGraph;


    struct PositionChange
    {
        double prevX;
        double prevY;
        double nextX;
        double nextY;
        double factor;
    };


    class BezierHandleItem: public QGraphicsItem
    {
        PositionChange nextChange;
        BezierPointItem* parentPoint;
        double f;

    public:
        BezierHandleItem(double factor,
                         BezierPointItem* parent);

        QRectF boundingRect() const override;

        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;

        QVariant itemChange(GraphicsItemChange change,
                            const QVariant &value)
                            override;

        void SendPositionChange(PositionChange change);
    };


    class BezierPointItem: public QGraphicsItem
    {
        friend BezierHandleItem;

        BezierGraph* view;
        BezierHandleItem* left;
        BezierHandleItem* right;
        bool locked = true;
        double f = 1000;

    public:
        BezierPointItem(BezierGraph* view, double factor,
                        QGraphicsItem* parent=nullptr);

        QRectF boundingRect() const override;

        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;

        bool Locked() const;

        void SetLocked(bool locked);

        BezierHandleItem* LeftHandle() const;

        BezierHandleItem* RightHandle() const;

    protected:
        void HandleMoved(BezierHandleItem* handle,
                         PositionChange change);
    };


    class BezierCurveItem: public QGraphicsPathItem
    {
    public:
        BezierCurveItem(QGraphicsItem* parent=nullptr);

        void Draw(BezierCurve* curve);
    };


    class BezierGraph: public QGraphicsView
    {
        friend BezierPointItem;

        BezierCurve* curve;
        QVector<BezierPointItem*> points;

    public:
        BezierGraph(BezierCurve* curve=nullptr,
                    QWidget* parent=nullptr);

        void Clear();

        void Init();

        void SetFunc(BezierCurve* curve);

        BezierCurve* Func() const;

    protected:
        void PointHandleChanged(BezierPointItem* item,
                                PositionChange change,
                                bool left=true);

        void PointChanged(BezierPointItem* item,
                          PositionChange change);
    };
}


#endif //GEX_UI_BEZIERFUNC_H
