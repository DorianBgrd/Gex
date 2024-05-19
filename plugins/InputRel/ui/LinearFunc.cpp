#include "LinearFunc.h"

#include <QLine>
#include <QResizeEvent>


Gex::InputRel::Handle::Handle(LinearGraph* g): QGraphicsEllipseItem()
{
    graph = g;

    setFlag(QGraphicsItem::ItemIsMovable);

    setFlag(QGraphicsItem::ItemIsSelectable);

    setBrush(QColor(200, 200, 200));
}


QVariant Gex::InputRel::Handle::itemChange(QGraphicsItem::GraphicsItemChange change,
                                           const QVariant &value)
{
    if (change == ItemPositionHasChanged)
    {
        graph->UpdateDraw();
    }

    return QGraphicsEllipseItem::itemChange(change, value);
}


Gex::InputRel::LinearGraph::LinearGraph(QWidget *parent): QWidget(parent)
{
    f = std::make_shared<LinearFunc>();

    auto* hl = new QHBoxLayout();
    setLayout(hl);

    sceneRect = QRect(0, 0, 1000, 1000);

    scene = new QGraphicsScene(sceneRect);
    view = new QGraphicsView(scene, this);
    view->fitInView(sceneRect);

    handle1 = new Handle(this);
    handle1->setRect(-10, -10, 20, 20);

    handle2 = new Handle(this);
    handle2->setRect(-10, -10, 20, 20);

    line = new QGraphicsLineItem();

    scene->addItem(handle1);
    scene->addItem(handle2);
    scene->addItem(line);

    handle1->setPos(0, sceneRect.height());
    handle2->setPos(sceneRect.width(),
                    sceneRect.height());

    hl->addWidget(view);
}


void Gex::InputRel::LinearGraph::Display(LinearFuncPtr func)
{
    f = func;

    double xmin = func->MinX();
    double xmax = func->MaxX();
    double ymin = func->MinY();
    double ymax = func->MaxY();

    double x0, x1, y0, y1;
    func->Get(x0, y0, x1, y1);

    // xmin -> 0;
    // xmax -> 1000;
    double xincr = (xmax - xmin) / 1000;
    double yincr = (ymax - ymin) / 1000;

    double start = xmin;
    double x0pos = (x0 - xmin) * 1000;
    double y0pos = 1000 - (y0 - ymin) * 1000;
    double x1pos = (x1 - xmin) * 1000;
    double y1pos = 1000 - (y0 - ymin) * 1000;
    // xmin -> 0
    // x    -> ?

    handle1->setPos(x0pos, y0pos);
    handle2->setPos(x1pos, y1pos);

    UpdateDraw();
}


void Gex::InputRel::LinearGraph::UpdateDraw()
{
    QLine l(handle1->pos().toPoint(),
            handle2->pos().toPoint());
    line->setLine(l);
}


void Gex::InputRel::LinearGraph::resizeEvent(QResizeEvent *event)
{
    view->fitInView(sceneRect);

    QWidget::resizeEvent(event);
}


QWidget* Gex::InputRel::LinearFuncWidget::CreateTypedWidget()
{
    graphView = new LinearGraph(this);
    return graphView;
}


void Gex::InputRel::LinearFuncWidget::SetValue(std::any value)
{
    graphView->Display(std::any_cast<LinearFuncPtr>(value));
}


std::any Gex::InputRel::LinearFuncWidget::GetValue() const
{
    return graphView->Func();
}


void Gex::InputRel::LinearFuncWidget::ShowConnected(bool connected)
{
    graphView->setDisabled(connected);
}


void Gex::InputRel::LinearFuncWidget::ShowDisabled(bool disabled)
{
    graphView->setDisabled(disabled);
}




QWidget* Gex::InputRel::LinearFuncInitWidget::CreateInitWidget()
{
    graphView = new LinearGraph();
    return graphView;
}


std::any Gex::InputRel::LinearFuncInitWidget::CreateValue()
{
    return std::make_any<LinearFuncPtr>(graphView->Func());
}


UiTSys::TypedInitWidget* Gex::InputRel::LinearFuncInitWidgetCreator::CreateWidget() const
{
    return new LinearFuncInitWidget();
}
