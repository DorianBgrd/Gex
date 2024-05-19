#ifndef GEX_LINEARFUNC_H
#define GEX_LINEARFUNC_H


#include "plugins/InputRel/Func.h"
#include "plugins/InputRel/Nodes.h"
#include "Gex/include/Gex.h"

#include "plugins/export.h"

#include "UiTsys/include/uitsys.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>


namespace Gex::InputRel
{
    class LinearGraph;

    class Plugin_API Handle: public QGraphicsEllipseItem
    {
        LinearGraph* graph;

    public:
        Handle(LinearGraph* graph);

        QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                            const QVariant &value) override;
    };

    class Plugin_API LinearGraph: public QWidget
    {
        QRect sceneRect;
        QGraphicsScene* scene;
        QGraphicsView* view;
        QGraphicsEllipseItem* handle1;
        QGraphicsEllipseItem* handle2;
        QGraphicsLineItem* line;

        LinearFuncPtr f;

    public:
        LinearGraph(QWidget* parent=nullptr);

        void Display(LinearFuncPtr func);

        void UpdateDraw();

        LinearFuncPtr Func() const {return f;}

        void resizeEvent(QResizeEvent *event) override;
    };


    struct Plugin_API LinearFuncWidget: public UiTSys::TypedWidget
    {
        LinearGraph* graphView = nullptr;

        virtual QWidget* CreateTypedWidget() override;

        virtual void SetValue(std::any value) override;

        virtual std::any GetValue() const override;

        virtual void ShowConnected(bool connected) override;

        virtual void ShowDisabled(bool disabled) override;
    };


    class Plugin_API LinearFuncWidgetCreator: public UiTSys::TypedWidgetCreator
    {
        UiTSys::TypedWidget* CreateWidget() const
        {
            return new LinearFuncWidget();
        }
    };


    class Plugin_API LinearFuncInitWidget: public UiTSys::TypedInitWidget
    {
        LinearGraph* graphView;
        public:

        virtual QWidget* CreateInitWidget() override;

        virtual std::any CreateValue() override;
    };


    class Plugin_API LinearFuncInitWidgetCreator: public UiTSys::TypedInitWidgetCreator
    {
        UiTSys::TypedInitWidget* CreateWidget() const override;
    };

}


#endif //GEX_LINEARFUNC_H
