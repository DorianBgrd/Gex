#include "plugins/InputRel/ui/BezierFunc.h"


Gex::InputRel::Handle::Handle(QGraphicsItem* parent): QGraphicsPolygonItem(parent)
{
    QList<QPointF> points = {
            QPointF(0, 5), QPointF(5, 0),
            QPointF(10, 5), QPointF(5, 10)
    };
    QPolygonF polygon(points);
    setPolygon(polygon);
}


Gex::InputRel::UIBezierPoint::UIBezierPoint(QGraphicsItem* parent): QGraphicsEllipseItem(parent)
{
    setRect(0, 0, 10, 10);
    setFlag(ItemIgnoresTransformations, true);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemIsMovable, true);

    setBrush(QColor(0, 0, 0));

    left = new Handle(this);
    right = new Handle(this);

    left->setPos(-50, 0);
    right->setPos(50, 0);
}


void Gex::InputRel::UIBezierPoint::paint(
        QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{
    QGraphicsEllipseItem::paint(painter, option, widget);

    QLineF leftLine(rect().center(),
                    left->pos());
    QLineF rightLine(rect().center(),
                     right->pos());

    painter->drawLine(leftLine);
    painter->drawLine(rightLine);
}


Gex::InputRel::Scene::Scene(QObject* parent): QGraphicsScene(parent)
{

}


Gex::InputRel::View::View(QWidget* parent): Gex::Ui::BaseGraphView(new Scene(), parent)
{
    scene()->addItem(new UIBezierPoint());

    auto* r = new QGraphicsRectItem(0, 0, 10, 10);
    r->setPos(10, 10);
    scene()->addItem(r);
}