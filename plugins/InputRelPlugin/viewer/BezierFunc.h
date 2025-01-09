#ifndef GEX_UI_BEZIERFUNC_H
#define GEX_UI_BEZIERFUNC_H


#include "plugins/InputRelPlugin/BezierFunc.h"
#include "plugins/InputRelPlugin/Nodes.h"
#include "Gex/include/Gex.h"

#include "plugins/export.h"

#include "UiTsys/include/uitsys.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>

#include "FuncViewer.h"

namespace Gex::InputRel
{
    class BezierFuncHandle;


    class BezierHandle: public QGraphicsObject
    {
    public:
        enum Side
        {
            Left,
            Right
        };

    private:
        bool init = false;
        Side side;
        BezierFuncHandle* point;
        BezierPoint::BezierHandleWk handle;

    public:
        BezierHandle(BezierPoint::BezierHandleWk h, Side side,
                     BezierFuncHandle* parent);

        QVariant itemChange(
                GraphicsItemChange change,
                const QVariant &value)
                override;

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;

        QRectF boundingRect() const override;
    };


    class BezierPointRepr: public QGraphicsEllipseItem
    {
    public:
        using QGraphicsEllipseItem::QGraphicsEllipseItem;

        QVariant itemChange(
                GraphicsItemChange change,
                const QVariant &value)
                override;
    };


    class BezierFuncHandle: public FuncViewerHandle
    {
        BezierHandle* leftHandle;
        BezierHandle* rightHandle;
        BezierPointRepr* repr;
        BezierPointWk bezierPnt;

    public:
        BezierFuncHandle(const CurvePointWk& point,
                         const FuncDelegatePtr& delegate,
                         FuncScene* scene);

        QRectF boundingRect() const override;

        void paintHandle(
                QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                QWidget *widget=nullptr)
                override;

        BezierPointWk BezierPoint() const;

        QVariant itemChange(GraphicsItemChange change,
                            const QVariant &value)
                            override;
    };


    class BezierFuncDelegate: public FuncViewerDelegate
    {
    private:
        BezierFuncPtr func;

    public:
        void AddPoint(double x, double y) override;

        void InitFunction(const std::any& f) override;

        PointsSet GetPoints() const override;

        FuncViewerHandle* CreateHandle(
                const CurvePointWk& p,
                const FuncDelegatePtr& d,
                FuncScene* scene)
        override;

        bool EditPoint(
                const CurvePointWk& p,
                const QPointF& previous,
                const QPointF& next,
                int decimals
        ) override;

//        void EditBezierPoint()

        void DrawCurve(
                QPainterPath& path,
                FuncScene* scene,
                int decimals
        ) const override;
    };


    struct BezierFuncDelegateCreator: public TypedDelegateCreator<BezierFuncDelegate> {};

}


#endif //GEX_UI_BEZIERFUNC_H
