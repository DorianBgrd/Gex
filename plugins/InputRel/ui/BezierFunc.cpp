#include "BezierFunc.h"


#include <QBrush>
#include <QPainter>
#include <QVector2D>


Gex::InputRel::BezierHandleItem::BezierHandleItem(
        PointRef handle_, double factor,
        BezierPointItem *parent):
    QGraphicsItem(parent)
{
    f = factor;
    handle = std::move(handle_);
    parentPoint = parent;
}


QRectF Gex::InputRel::BezierHandleItem::boundingRect() const
{
    return QRectF(0, 0, f, f);
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

    if (change == QGraphicsItem::ItemPositionChange)
    {
        nextChange.prevX = p.x();
        nextChange.prevY = p.y();
        nextChange.factor = f;
    }
    else if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        nextChange.nextX = p.x();
        nextChange.nextY = p.y();

        handle.lock()->SetX(p.x() / f);
        handle.lock()->SetY(p.y() / f);

        SendPositionChange(nextChange);
    }

    return QGraphicsItem::itemChange(change, value);
}



Gex::InputRel::BezierPointItem::BezierPointItem(BezierPointRef point_,
                                                BezierGraph* view_,
                                                double factor,
                                                QGraphicsItem* parent):
    QGraphicsItem(parent)
{
    point = std::move(point_);
    f = factor;
    view = view_;
    left = new BezierHandleItem(point.lock()->LeftHandle(), f, this);
    right = new BezierHandleItem(point.lock()->RightHandle(), f, this);
}


QRectF Gex::InputRel::BezierPointItem::boundingRect() const
{
    return QRectF(0, 0, 1 * f, 1 * f);
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
    }
}


Gex::InputRel::BezierCurveItem::BezierCurveItem(
        double factor, QGraphicsItem* parent):
        QGraphicsPathItem(parent)
{
    f = factor;
    QPen pen(QColor(255, 0, 0));
    pen.setWidth(f);
}


void Gex::InputRel::BezierCurveItem::Draw(BezierFuncPtr curve)
{
    QPainterPath path;

    auto curves = curve->CurvePoints();

    if (curves.empty())
        return;

    auto previous = curves.begin();

    auto previousPoint = *previous;
    path.moveTo(previousPoint->GetX() * f, previousPoint->GetY() * f);

    previous++;
    auto next = previous;
    for (;next != curves.end(); next++)
    {
        auto nextPoint = *next;

        QPoint cp1(previousPoint->RightHandle().lock()->GetX() * f,
                   previousPoint->RightHandle().lock()->GetY() * f);

        QPoint cp2(nextPoint->LeftHandle().lock()->GetX() * f,
                   nextPoint->LeftHandle().lock()->GetY() * f);

        QPoint n(nextPoint->GetX() * f,
                 nextPoint->GetY() * f);

        path.cubicTo(cp1, cp2, n);
        path.moveTo(n);

        previous = next;
    }
}


Gex::InputRel::BezierGraph::BezierGraph(BezierFunc* crv, QWidget* parent): QGraphicsView(parent)
{
    if (!crv)
        curve = std::make_shared<BezierFunc>();

    auto* gscene = new QGraphicsScene();
    setScene(gscene);

    curveItem = new BezierCurveItem(f);
    gscene->addItem(curveItem);

    Init();
}


void Gex::InputRel::BezierGraph::resizeEvent(QResizeEvent* event)
{
    fitInView(sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
}


void Gex::InputRel::BezierGraph::Clear()
{
    for (auto p : points)
    {
        scene()->removeItem(p);
        delete p;
    }

    curveItem->Draw(curve);
}


Gex::InputRel::BezierPointItem* Gex::InputRel::BezierGraph::InitPointItem(BezierPointRef p)
{
    auto* point = new BezierPointItem(p, this, f);

    scene()->addItem(point);

    auto sp = p.lock();

    point->setPos(sp->GetX() * f,
                  sp->GetY() * f);

    point->LeftHandle()->setPos(sp->LeftHandle().lock()->GetX() * f,
                                sp->LeftHandle().lock()->GetY() * f);

    point->RightHandle()->setPos(sp->RightHandle().lock()->GetX() * f,
                                 sp->RightHandle().lock()->GetY() * f);

    return point;
}


void Gex::InputRel::BezierGraph::Init()
{
    Clear();

    for (auto p : curve->CurvePointRefs())
    {
        points.append(InitPointItem(p));
    }
}


void Gex::InputRel::BezierGraph::SetFunc(BezierFuncPtr crv)
{
    curve = crv;

    Init();
}

Gex::InputRel::BezierFuncPtr Gex::InputRel::BezierGraph::Func() const
{
    return curve;
}


void Gex::InputRel::BezierGraph::AddPoint(int x)
{
    double vx = (x / f);

    auto point = curve->AddPoint(vx);

    auto* p = InitPointItem(point);

    points.append(p);

    scene()->addItem(p);

    curveItem->Draw(curve);
}


void Gex::InputRel::BezierGraph::RemoveSelectedPoints()
{
    auto sel = scene()->selectedItems();
    for (auto* item : sel)
    {
        auto* pt = dynamic_cast<BezierPointItem*>(item);
        if (!pt)
            continue;

        scene()->removeItem(pt);
        delete pt;
    }

    curveItem->Draw(curve);
}





QWidget* Gex::InputRel::BezierFuncWidget::CreateTypedWidget()
{
    graphView = new BezierGraph(nullptr, this);
    return graphView;
}


void Gex::InputRel::BezierFuncWidget::SetValue(std::any value)
{
    graphView->SetFunc(std::any_cast<BezierFuncPtr>(value));
}


std::any Gex::InputRel::BezierFuncWidget::GetValue() const
{
    return graphView->Func();
}


void Gex::InputRel::BezierFuncWidget::ShowConnected(bool connected)
{
    graphView->setDisabled(connected);
}


void Gex::InputRel::BezierFuncWidget::ShowDisabled(bool disabled)
{
    graphView->setDisabled(disabled);
}




QWidget* Gex::InputRel::BezierFuncInitWidget::CreateInitWidget()
{
    graphView = new BezierGraph();
    return graphView;
}


std::any Gex::InputRel::BezierFuncInitWidget::CreateValue()
{
    return std::make_any<BezierFuncPtr>(graphView->Func());
}


UiTSys::TypedInitWidget* Gex::InputRel::BezierFuncInitWidgetCreator::CreateWidget() const
{
    return new BezierFuncInitWidget();
}
