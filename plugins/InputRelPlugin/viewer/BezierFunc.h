#ifndef GEX_UI_BEZIERFUNC_H
#define GEX_UI_BEZIERFUNC_H

#include "plugins/InputRel/BezierFunc.h"

#include "plugins/export.h"

#include "UiTsys/include/uitsys.h"

#include "ui/include/BaseGraph.h"


#include <QPainterPath>

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>


namespace Gex::InputRel
{
    typedef QGraphicsEllipseItem PointItem;

    class Handle: public QGraphicsPolygonItem
    {
    public:
        Handle(QGraphicsItem* item=nullptr);
    };

    class UIBezierPoint: public QGraphicsEllipseItem
    {
        Handle* left;
        Handle* right;
    public:
        UIBezierPoint(QGraphicsItem* parent=nullptr);

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;
    };


    class Scene: public QGraphicsScene
    {
    public:
        Scene(QObject* parent=nullptr);
    };


    class View: public Gex::Ui::BaseGraphView
    {
    public:
        View(QWidget* parent=nullptr);
    };

/*
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
    */
}


#endif //GEX_UI_BEZIERFUNC_H
