#include "BezierFunc.h"


#include <QBrush>
#include <QPainter>
#include <QVector2D>


Gex::InputRel::BezierHandleItem::BezierHandleItem(
        double factor, BezierPointItem *parent):
    QGraphicsItem(parent)
{
    f = factor;
    parentPoint = parent;
}


QRectF Gex::InputRel::BezierHandleItem::boundingRect() const
{
    return QRectF(0, 0, 10 * f, 10 * f);
}


void Gex::InputRel::BezierHandleItem::paint(QPainter *painter,
                                            const QStyleOptionGraphicsItem *option,
                                            QWidget *widget)
{
    QPointF parentCenter = mapFromParent(parentItem()->boundingRect().center());

    painter->save();

    QBrush brush(QColor(255, 0, 0));
    painter->setBrush(brush);

    QRectF r = boundingRect();
    QPoint points[] = {
            QPoint(r.left(), r.center().x()),
            QPoint(r.center().x(), r.top()),
            QPoint(r.right(), r.center().x()),
            QPoint(r.center().x(), r.bottom()),
            };

    painter->drawPolygon(points, 4);

    QBrush transp = Qt::NoBrush;
    painter->setBrush(transp);

    QPen pen(QColor(255, 0, 0));
    pen.setWidth(2);
    painter->setPen(pen);

    painter->drawLine(parentCenter, r.center());

    painter->restore();
}


void Gex::InputRel::BezierHandleItem::SendPositionChange(
        PositionChange change)
{
    parentPoint->HandleMoved(this, change);
}


QVariant Gex::InputRel::BezierHandleItem::itemChange(
        GraphicsItemChange change, const QVariant &value)
{
    QPointF p = pos();

    nextChange.factor = f;
    if (change == QGraphicsItem::ItemPositionChange)
    {
        nextChange.prevX = p.x();
        nextChange.prevY = p.y();
    }
    else if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        nextChange.nextX = p.x();
        nextChange.nextY = p.y();

        SendPositionChange(nextChange);
    }

    return QGraphicsItem::itemChange(change, value);
}



Gex::InputRel::BezierPointItem::BezierPointItem(BezierGraph* view_,
                                                double factor,
                                                QGraphicsItem* parent):
    QGraphicsItem(parent)
{
    f = factor;
    view = view_;
    left = new BezierHandleItem(f, this);
    right = new BezierHandleItem(f, this);
}


QRectF Gex::InputRel::BezierPointItem::boundingRect() const
{
    return QRectF(0, 0, 15 * f, 15 * f);
}


void Gex::InputRel::BezierPointItem::paint(QPainter *painter,
                                           const QStyleOptionGraphicsItem *option,
                                           QWidget *widget)
{
    painter->save();

    QBrush brush(QColor(255, 0, 0));
    painter->setBrush(brush);

    QRectF r = boundingRect();
    QPoint points[] = {
            QPoint(r.left(), r.center().x()),
            QPoint(r.center().x(), r.top()),
            QPoint(r.right(), r.center().x()),
            QPoint(r.center().x(), r.bottom()),
    };

    painter->drawPolygon(points, 4);
    painter->restore();
}


bool Gex::InputRel::BezierPointItem::Locked() const
{
    return locked;
}


void Gex::InputRel::BezierPointItem::SetLocked(bool l)
{
    locked = l;
}


Gex::InputRel::BezierHandleItem* Gex::InputRel::BezierPointItem::LeftHandle() const
{
    return left;
}


Gex::InputRel::BezierHandleItem* Gex::InputRel::BezierPointItem::RightHandle() const
{
    return right;
}


void Gex::InputRel::BezierPointItem::HandleMoved(
        BezierHandleItem* handle, PositionChange change)
{
    PositionChange handleChange;

    bool isLeft = (handle == left);
    if (locked)
    {
        BezierHandleItem* sourceHandle = handle;
        BezierHandleItem* destHandle = (isLeft ? right : left);

        double cx = boundingRect().center().x();
        double cy = boundingRect().center().y();
        double dx = destHandle->pos().x();
        double dy = destHandle->pos().y();
        double sx = sourceHandle->pos().x();
        double sy = sourceHandle->pos().y();
        double distance = std::sqrt(std::pow(cx - dx, 2) + std::pow(cy - dy, 2));

        QPoint center(cx, cy);

        QVector2D dir(sx - cx, sy - cy);
        dir.normalize();

        QPoint dest(center.x() + distance * dir.x(),
                    center.y() + distance * dir.y());

        destHandle->setPos(dest);

        double ndx = dest.x();
        double ndy = dest.y();
        handleChange = {dx, dy, ndx, ndy, f};
    }

    view->PointHandleChanged(this, change, isLeft);
    if (locked)
    {
        view->PointHandleChanged(this, handleChange, !isLeft);
    }
}


Gex::InputRel::BezierCurveItem::BezierCurveItem(
        QGraphicsItem* parent): QGraphicsPathItem(parent)
{

}


void Gex::InputRel::BezierCurveItem::Draw(BezierCurve* curve)
{
    QPainterPath path;

    auto cmap = curve->CurvePointsMap();

    auto previous = cmap.begin();

    auto previousPoint = *previous;
    path.moveTo(previousPoint.first.x, previousPoint.first.y);

    previous++;
    auto next = previous;
    for (;next != cmap.end(); next++)
    {
        QPoint cp1(previous->second.cp2.x,
                   previous->second.cp2.y);

        QPoint cp2(next->second.cp1.x,
                   next->second.cp1.y);

        QPoint n(next->first.x,
                 next->first.y);

        path.cubicTo(cp1, cp2, n);
        path.moveTo(n);

        previous = next;
    }
}


Gex::InputRel::BezierGraph::BezierGraph(BezierCurve* crv, QWidget* parent): QGraphicsView(parent)
{
    if (!crv)
        curve = new BezierCurve();

    auto* gscene = new QGraphicsScene();
    setScene(gscene);

    Init();
}


void Gex::InputRel::BezierGraph::Clear()
{
    for (auto p : points)
    {
        scene()->removeItem(p);
        delete p;
    }
}


void Gex::InputRel::BezierGraph::Init()
{
    Clear();

    for (auto p : curve->CurvePointsMap())
    {
        auto point = new BezierPointItem(this, f);
        point->setPos(p.first.x,
                      p.first.y);
        point->LeftHandle()->setPos(p.second.cp1.x,
                                    p.second.cp1.y);
        point->RightHandle()->setPos(p.second.cp2.x,
                                    p.second.cp2.y);

        points.append(point);
    }
}


void Gex::InputRel::BezierGraph::SetFunc(BezierCurve* crv)
{
    curve = crv;

    Init();
}

Gex::InputRel::BezierCurve* Gex::InputRel::BezierGraph::Func() const
{
    return curve;
}


void Gex::InputRel::BezierGraph::PointHandleChanged(
        BezierPointItem* item, PositionChange change,
        bool left)
{

}


void Gex::InputRel::BezierGraph::PointChanged(
        BezierPointItem* item, PositionChange change)
{

}

