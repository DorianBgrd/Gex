#include "BezierFunc.h"

#include "FuncViewer.h"

#include <numbers>


Gex::InputRel::BezierHandle::BezierHandle(
        BezierPoint::BezierHandleWk h,
        Side s, BezierFuncHandle* parent
        ): QGraphicsObject(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    handle = h;
    point = parent;
    side = s;

//    setBrush(Qt::yellow);
//    setPen(Qt::NoPen);

    setPos(ToViewX(handle->hx, point->Delegate()->Decimals()),
           ToViewY(handle->hy, point->Delegate()->Decimals()));

//    setRect(QRectF(-4, -4, 8, 8));

    init = true;
}


double Angle(
        const QPointF& vector1,
        const QPointF& vector2
)
{
//    double v1Length = Distance(p1.x, p1.y, p2.x, p2.y);
//    double v2Length = Distance(p1.x, p1.y, p3.x, p3.y);

    double v1Norm = std::sqrt(std::pow(vector1.x(), 2) + std::pow(vector1.y(), 2));
    double v2Norm = std::sqrt(std::pow(vector2.x(), 2) + std::pow(vector2.y(), 2));

    double scalarProduct = vector1.x() * vector2.x() + vector1.y() * vector2.y();

    double p = scalarProduct / (v1Norm * v2Norm);

    double ac = std::acos(p);
//    return std::acos((v1.x * v2.x) + (v1.y * v2.y));
    return ac;
}


#define PI 3.14159265359
#define RAD 57.2958


void Gex::InputRel::BezierHandle::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{
    painter->setBrush(Qt::yellow);
    if (isSelected())
        painter->setBrush(Qt::white);

    painter->setPen(Qt::NoPen);

    QPointF p = pos();
    QPointF xvec(1, 0);
    QPointF vec = QPointF(0, 0) - p;

    double angle = Angle(xvec, vec) * 57.2958;

    if (p.y() > 0)
    {
        angle *= -1;
    }

    painter->rotate(angle);
    painter->drawRect(boundingRect());
}


QRectF Gex::InputRel::BezierHandle::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}


QVariant Gex::InputRel::BezierHandle::itemChange(
        GraphicsItemChange change,
        const QVariant &value)
{
    if (!init)
    {
        return value;
    }

    if (change == QGraphicsItem::ItemPositionChange)
    {
        auto pos = qvariant_cast<QPointF>(value);

        if (side == Side::Left)
        {
            if (pos.x() >= 0)
            {
                pos.setX(0);
            }
        }
        else if (side == Side::Right)
        {
            if (pos.x() <= 0)
            {
                pos.setX(0);
            }
        }

        return QVariant(pos);
    }
    if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        auto bezierPnt = point->BezierPoint();
        BezierPoint::BezierHandleWk handle;
        if (side == Side::Left)
        {
            handle = bezierPnt->LeftHandle();
        }
        else if (side == Side::Right)
        {
            handle = bezierPnt->RightHandle();
        }

        if (handle)
        {
            QPointF cpos = scenePos();
            QPointF ppos = point->scenePos();
            QPointF spos = cpos - ppos;
            handle->hx = FromViewX(
                    spos.x(),
                    point->Delegate()->Decimals()
            );

            handle->hy = FromViewY(
                    spos.y(),
                    point->Delegate()->Decimals()
            );

            point->OnModified();
        }
    }

    return value;
}


//void Gex::InputRel::BezierHandle::paint(
//        QPainter *painter,
//        const QStyleOptionGraphicsItem *option,
//        QWidget *widget
//)
//{
//    painter->setBrush(Qt::yellow);
//    painter->setPen(Qt::NoPen);
//
//    painter->drawRect(boundingRect());
//}


//QRectF Gex::InputRel::BezierHandle::boundingRect() const
//{
//    return QRectF(-4, -4, 8, 8);
//}


QVariant Gex::InputRel::BezierPointRepr::itemChange(
        GraphicsItemChange change,
        const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        bool sel = qvariant_cast<bool>(value);
        parentItem()->setSelected(sel);

        return QVariant(false);
    }

    return value;
}


Gex::InputRel::BezierFuncHandle::BezierFuncHandle(
        const CurvePointWk& point,
        const FuncDelegatePtr& delegate,
        FuncScene* scene
): FuncViewerHandle(point, delegate, scene)
{
    setFlag(ItemIgnoresTransformations, false);
    setFlag(ItemIsSelectable, true);

    repr = new BezierPointRepr(
            QRectF(-6, -6, 12, 12),
            this);
    repr->setBrush(Qt::yellow);
    repr->setPen(Qt::NoPen);
    repr->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    repr->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    repr->setFlag(QGraphicsItem::ItemIsSelectable);
    repr->setFlag(QGraphicsItem::ItemIsMovable);

    bezierPnt = std::dynamic_pointer_cast<
            Gex::InputRel::BezierPoint,
            Gex::InputRel::CurvePoint>(
                    point.ToShared()
                    );

    leftHandle = new BezierHandle(
            bezierPnt->LeftHandle(),
            BezierHandle::Side::Left,
            this);

    rightHandle = new BezierHandle(
            bezierPnt->RightHandle(),
            BezierHandle::Side::Right,
            this);
}


QRectF Gex::InputRel::BezierFuncHandle::boundingRect() const
{
    return repr->sceneBoundingRect();
//    QPointF leftHandlePos = leftHandle->scenePos();
//    QPointF rightHandlePos = rightHandle->scenePos();
//    QPointF reprPos = repr->scenePos();
//
//    return QRectF(
//        QPointF(std::min({leftHandlePos.x(), rightHandlePos.x(), reprPos.x()}),
//                std::min({leftHandlePos.y(), rightHandlePos.y(), reprPos.x()})),
//        QPointF(std::max({leftHandlePos.x(), rightHandlePos.x(), reprPos.x()}),
//                std::max({rightHandlePos.y(), rightHandlePos.y(), reprPos.x()}))
//    );
}


void Gex::InputRel::BezierFuncHandle::paintHandle(
        QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{
    QPen linePen(Qt::yellow);
    linePen.setWidth(0);

    painter->setPen(linePen);
    painter->setBrush(Qt::NoBrush);

    painter->drawLine(
            leftHandle->pos(),
            {0, 0}
    );

    painter->drawLine(
            rightHandle->pos(),
            {0, 0}
    );
}


Gex::InputRel::BezierPointWk Gex::InputRel::BezierFuncHandle::BezierPoint() const
{
    return bezierPnt;
}


QVariant Gex::InputRel::BezierFuncHandle::itemChange(
        GraphicsItemChange change,
        const QVariant &value
)
{
    if (change == ItemSelectedChange)
    {
        QBrush br(Qt::yellow);
        if (qvariant_cast<bool>(value))
            br.setColor(Qt::white);

        repr->setBrush(br);
    }

    return FuncViewerHandle::itemChange(change, value);
}


void Gex::InputRel::BezierFuncDelegate::AddPoint(double x, double y)
{
    func->AddPoint(x, y);
}


void Gex::InputRel::BezierFuncDelegate::InitFunction(const std::any& f)
{
    func = std::any_cast<BezierFuncPtr>(f);
}


Gex::InputRel::PointsSet Gex::InputRel::BezierFuncDelegate::GetPoints() const
{
    return func->Points();
}


Gex::InputRel::FuncViewerHandle* Gex::InputRel::BezierFuncDelegate::CreateHandle(
        const CurvePointWk& p,
        const FuncDelegatePtr& d,
        FuncScene* scene)
{
    return new BezierFuncHandle(p, d, scene);
}


bool Gex::InputRel::BezierFuncDelegate::EditPoint(
        const CurvePointWk& p,
        const QPointF& previous,
        const QPointF& next,
        int decimals
)
{
    PointDelta delta;
    delta.SetDeltaX(FromViewX(next.x() - previous.x(), decimals));
    delta.SetDeltaY(FromViewY(next.y() - previous.y(), decimals));

    return func->EditPoint(p->GetX(), &delta);
}


void Gex::InputRel::BezierFuncDelegate::DrawCurve(
        QPainterPath& path,
        FuncScene* scene,
        int decimals
) const
{
    BezierPointPtr previousPoint;
    for (const CurvePointPtr& point : func->Points())
    {
        BezierPointPtr bezierPoint = std::dynamic_pointer_cast<
                BezierPoint, CurvePoint>(point);
        if (!bezierPoint)
            continue;

        if (!previousPoint)
        {
            previousPoint = bezierPoint;
            continue;
        }

        path.moveTo(ToViewX(previousPoint->GetX(), decimals),
                    ToViewY(previousPoint->GetY(), decimals));

        auto rightHandle = previousPoint->RightHandle();
        auto leftHandle = bezierPoint->LeftHandle();

        path.moveTo(ToViewX(previousPoint->GetX(), decimals),
                    ToViewY(previousPoint->GetY(), decimals));

        double bx = previousPoint->GetX() + rightHandle->hx;
        if (bx > bezierPoint->GetX())
            bx = bezierPoint->GetX();

        double cx = bezierPoint->GetX() + leftHandle->hx;
        if (cx < previousPoint->GetX())
            cx = previousPoint->GetX();

        path.cubicTo(
                ToViewX(bx, decimals),
                ToViewY(previousPoint->GetY() + rightHandle->hy, decimals),
                ToViewX(cx, decimals),
                ToViewY(bezierPoint->GetY() + leftHandle->hy, decimals),
                ToViewX(bezierPoint->GetX(), decimals),
                ToViewY(bezierPoint->GetY(), decimals)
                );

        for (double t = 0; t < 1; t += 0.1)
        {
            QPointF bp(
                    ToViewX(func->BezierX(previousPoint, bezierPoint, t), decimals),
                    ToViewY(func->BezierY(previousPoint, bezierPoint, t), decimals)
            );

            path.addRect(
                    bp.x() - ToViewX(2, decimals),
                    bp.y() + ToViewY(2, decimals),
                    ToViewLength(4, decimals),
                    ToViewLength(4, decimals)
            );
        }

        previousPoint = bezierPoint;
    }
}
