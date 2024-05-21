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
        PointRef handle;
        BezierPointItem* parentPoint;
        PositionChange nextChange;
        double f;

    public:
        BezierHandleItem(PointRef handle, double factor,
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

        BezierPointRef point;
        BezierGraph* view;
        BezierHandleItem* left;
        BezierHandleItem* right;
        bool locked = true;
        double f = 1000;

    public:
        BezierPointItem(BezierPointRef point,
                        BezierGraph* view, double factor,
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
        double f;
    public:
        BezierCurveItem(double factor, QGraphicsItem* parent=nullptr);

        void Draw(BezierFuncPtr curve);
    };


    class BezierGraph: public QGraphicsView
    {
        friend BezierPointItem;

        double f = 1000;
        BezierFuncPtr curve;
        QVector<BezierPointItem*> points;
        BezierCurveItem* curveItem;

    public:
        BezierGraph(BezierFunc* curve=nullptr,
                    QWidget* parent=nullptr);

        void resizeEvent(QResizeEvent* event) override;

        void Clear();

        void Init();

        void SetFunc(BezierFuncPtr curve);

        BezierFuncPtr Func() const;

        void AddPoint(int x);

        void RemoveSelectedPoints();

        BezierPointItem* InitPointItem(BezierPointRef p);
    };


    struct Plugin_API BezierFuncWidget: public UiTSys::TypedWidget
    {
        BezierGraph* graphView = nullptr;

        virtual QWidget* CreateTypedWidget() override;

        virtual void SetValue(std::any value) override;

        virtual std::any GetValue() const override;

        virtual void ShowConnected(bool connected) override;

        virtual void ShowDisabled(bool disabled) override;
    };


    class Plugin_API BezierFuncWidgetCreator: public UiTSys::TypedWidgetCreator
    {
        UiTSys::TypedWidget* CreateWidget() const
        {
            return new BezierFuncWidget();
        }
    };


    class Plugin_API BezierFuncInitWidget: public UiTSys::TypedInitWidget
    {
        BezierGraph* graphView;
    public:

        virtual QWidget* CreateInitWidget() override;

        virtual std::any CreateValue() override;
    };


    class Plugin_API BezierFuncInitWidgetCreator: public UiTSys::TypedInitWidgetCreator
    {
        UiTSys::TypedInitWidget* CreateWidget() const override;
    };
}


#endif //GEX_UI_BEZIERFUNC_H
